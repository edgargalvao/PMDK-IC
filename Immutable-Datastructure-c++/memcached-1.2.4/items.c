/* -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* $Id: items.c 642 2007-11-16 09:36:07Z dormando $ */
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <time.h>
#include <memcached.h>
#include <x86intrin.h>

#define CACHE_LINE_SIZE 64
void clwb_range(const void *ptr, uint64_t len) {
    uintptr_t start = (uintptr_t)ptr & ~(CACHE_LINE_SIZE-1);
    for (; start < (uintptr_t)ptr + len; start += CACHE_LINE_SIZE) {
        _mm_clwb((void*)start);
    }
}

void sfence() {
        asm volatile("sfence":::"memory");
}

/* Forward Declarations */

void item_link_q(item *it);

void item_unlink_q(item *it);

uint64_t get_cas_id();

/*
 * We only reposition items in the LRU queue if they haven't been repositioned
 * in this many seconds. That saves us from churning on frequently-accessed
 * items.
 */
#define ITEM_UPDATE_INTERVAL 60

#define LARGEST_ID 255
// This is an array of linked lists!
static item* heads[LARGEST_ID];
static item* tails[LARGEST_ID];
static unsigned int sizes[LARGEST_ID];

#define POWER_SMALLEST 1
#define POWER_LARGEST 200
#define POWER_BLOCK 1048576 // slab size = 1MB
#define CHUNK_ALIGN_BYTES (sizeof(void *))

int slab_sizes[POWER_LARGEST+1];

/**
 * We dont have a slab allocator but memcached relies heavily on slab class id.
 */
void slabs_init(const size_t limit, const double factor) {
    int i = POWER_SMALLEST - 1;
    unsigned int size = sizeof(item) + settings.chunk_size;

    /* Factor of 2.0 means use the default memcached behavior */
	// If factor is 2 then the chunk size defaults to 128, Sanketh
    if (factor == 2.0 && size < 128)
        size = 128;

    while (++i < POWER_LARGEST && size <= POWER_BLOCK / 2) {
        /* Make sure items are always n-byte aligned */
        if (size % CHUNK_ALIGN_BYTES)
            size += CHUNK_ALIGN_BYTES - (size % CHUNK_ALIGN_BYTES);

        slab_sizes[i] = size;
        size *= factor;
    }
    slab_sizes[POWER_LARGEST] = POWER_BLOCK;
}

unsigned int slabs_clsid(const size_t size) {
    if (size == 0)
        return 0;
    int res = POWER_SMALLEST; 
    while (size > slab_sizes[res])
        if (res++ == POWER_LARGEST)
            return 0;
    return res;
}

void item_init(void) {
    int i;
    for(i = 0; i < LARGEST_ID; i++) {
        heads[i] = NULL;
        tails[i] = NULL;
        sizes[i] = 0;
    }
}

/* Get the next CAS id for a new item. */

uint64_t get_cas_id() {
    static uint64_t cas_id = 0;
    return ++cas_id;
}

/* Enable this for reference-count debugging. */
#if 0
# define DEBUG_REFCNT(it,op) \
                fprintf(stderr, "item %x refcnt(%c) %d %c%c%c\n", \
                        it, op, it->refcount, \
                        (it->it_flags & ITEM_LINKED) ? 'L' : ' ', \
                        (it->it_flags & ITEM_SLABBED) ? 'S' : ' ', \
                        (it->it_flags & ITEM_DELETED) ? 'D' : ' ')
#else
# define DEBUG_REFCNT(it,op) while(0)
#endif

/**
 * Generates the variable-sized part of the header for an object.
 *
 * key     - The key
 * nkey    - The length of the key
 * flags   - key flags
 * nbytes  - Number of bytes to hold value and addition CRLF terminator
 * suffix  - Buffer for the "VALUE" line suffix (flags, size).
 * nsuffix - The length of the suffix is stored here.
 *
 * Returns the total size of the header.
 */


size_t item_make_header(const uint8_t nkey, const int flags, const int nbytes,
                     char *suffix, uint8_t *nsuffix) {
    /* suffix is defined at 40 chars elsewhere.. */
    *nsuffix = (uint8_t) snprintf(suffix, 40, " %d %d\r\n", flags, nbytes - 2);
    return sizeof(item) + nkey + *nsuffix + nbytes;
}

/*@null@*/

item *do_item_alloc(char *key, const size_t nkey, const int flags, const rel_time_t exptime, const int nbytes) {

    /* nbytes is simply the size of a value and not the value itself ! */

    uint8_t nsuffix;
    item *it;
    char suffix[40];
    size_t ntotal = item_make_header(nkey + 1, flags, nbytes, suffix, &nsuffix);

    unsigned int id = slabs_clsid(ntotal);
    if (id == 0)
        return 0;

    it = (item*) nvm_reserve(ntotal);
    if (it == 0) {
        int tries = 50;
        item *search;

        /* If requested to not push old items out of cache when memory runs out,
         * we're out of luck at this point...
         */

        if (settings.evict_to_free == 0) return NULL;

        /*
         * try to get one off the right LRU
         * don't necessariuly unlink the tail because it may be locked: refcount>0
         * search up from tail an item with refcount==0 and unlink it; give up after 50
         * tries
         */

        if (id > LARGEST_ID) return NULL;
        if (tails[id] == 0) return NULL;

	// LRU right here...the fabled and mysterious LRU !!! 
        for (search = tails[id]; tries > 0 && search != NULL; tries--, search=search->prev) {
            if (search->refcount == 0) {
               if (search->exptime == 0 || search->exptime > current_time) {
                       STATS_LOCK();
                       	stats.evictions++;
                       STATS_UNLOCK();
                }
                do_item_unlink(search);
                break;
            }
        }
        it = (item*) nvm_reserve(ntotal);
        if (it == 0) return NULL;
    }

    assert(it->slabs_clsid == 0);

    it->slabs_clsid = id;

    assert(it != heads[it->slabs_clsid]);

	// All references to item are to persistent memory !!!!
	// But unfortunately u wont see them in 
	// this routine in the assembly, they'll be in 
	// some deep routine in mnemosyne
    it->next = it->prev = it->h_next = 0;
    it->refcount = 1;     /* the caller will have a reference */
    DEBUG_REFCNT(it, '*');
    it->it_flags = 0;
    it->nkey = nkey;
    it->nbytes = nbytes;
	/* sanketh, possible ref to persistent memory */

    strcpy(ITEM_key(it), key);
    it->exptime = exptime;
    memcpy(ITEM_suffix(it), suffix, (size_t)nsuffix);
    it->nsuffix = nsuffix;
    return it;
}


void item_free(item *it) {
    size_t ntotal = ITEM_ntotal(it);
    assert((it->it_flags & ITEM_LINKED) == 0);
    assert(it != heads[it->slabs_clsid]);
    assert(it != tails[it->slabs_clsid]);
    assert(it->refcount == 0);

    /* so slab size changer can tell later if item is already free or not */
    it->slabs_clsid = 0;
    it->it_flags |= ITEM_SLABBED;
    DEBUG_REFCNT(it, 'F');
    nvm_free(it, NULL, NULL, NULL, NULL);
}

/**
 * Returns true if an item will fit in the cache (its size does not exceed
 * the maximum for a cache entry.)
 */
bool item_size_ok(const size_t nkey, const int flags, const int nbytes) {
    char prefix[40];
    uint8_t nsuffix;

    return slabs_clsid(item_make_header(nkey + 1, flags, nbytes,
                                        prefix, &nsuffix)) != 0;
}


void item_link_q(item *it) { /* item is the new head */
    item **head, **tail;
    /* always true, warns: assert(it->slabs_clsid <= LARGEST_ID); */
    assert((it->it_flags & ITEM_SLABBED) == 0);

    head = &heads[it->slabs_clsid];
    tail = &tails[it->slabs_clsid];
    assert(it != *head);
    assert((*head && *tail) || (*head == 0 && *tail == 0));
    it->prev = 0;
    it->next = *head;
    if (it->next) it->next->prev = it;
    *head = it;
    if (*tail == 0) *tail = it;
    sizes[it->slabs_clsid]++;
    return;
}


void item_unlink_q(item *it) {
    item **head, **tail;
    /* always true, warns: assert(it->slabs_clsid <= LARGEST_ID); */
    head = &heads[it->slabs_clsid];
    tail = &tails[it->slabs_clsid];

    if (*head == it) {
        assert(it->prev == 0);
        *head = it->next;
    }
    if (*tail == it) {
        assert(it->next == 0);
        *tail = it->prev;
    }
    assert(it->next != it);
    assert(it->prev != it);

    if (it->next) it->next->prev = it->prev;
    if (it->prev) it->prev->next = it->next;
    sizes[it->slabs_clsid]--;
    return;
}


int do_item_link(item *it) {
    assert((it->it_flags & (ITEM_LINKED|ITEM_SLABBED)) == 0);
    assert(it->nbytes < (1024 * 1024));  /* 1MB max size */
    it->it_flags |= ITEM_LINKED;
    it->time = current_time;
	// inserting into the hash table !!!
    assoc_insert(it);

    STATS_LOCK();
	{
    	stats.curr_bytes += ITEM_ntotal(it);
	    stats.curr_items += 1;
    	stats.total_items += 1;
	}	
    STATS_UNLOCK();

    /* Allocate a new CAS ID on link. */
    it->cas_id = get_cas_id();

    item_link_q(it);

    return 1;
}


void do_item_unlink(item *it) {
    if ((it->it_flags & ITEM_LINKED) != 0) {
        it->it_flags &= ~ITEM_LINKED;
        STATS_LOCK();
		{
        	stats.curr_bytes -= ITEM_ntotal(it);
        	stats.curr_items -= 1;
		}
        STATS_UNLOCK();
        assoc_delete(ITEM_key(it), it->nkey);
        item_unlink_q(it);
        if (it->refcount == 0) item_free(it);
    }
}


void do_item_remove(item *it) {
    assert((it->it_flags & ITEM_SLABBED) == 0);
    if (it->refcount != 0) {
        it->refcount--;
        DEBUG_REFCNT(it, '-');
    }
    assert((it->it_flags & ITEM_DELETED) == 0 || it->refcount != 0);
    if (it->refcount == 0 && (it->it_flags & ITEM_LINKED) == 0) {
        item_free(it);
    }
}


void do_item_update(item *it) {
    if (it->time < current_time - ITEM_UPDATE_INTERVAL) {
        assert((it->it_flags & ITEM_SLABBED) == 0);

        if ((it->it_flags & ITEM_LINKED) != 0) {
            item_unlink_q(it);
            it->time = current_time;
            item_link_q(it);
        }
    }
}


int do_item_replace(item *it, item *new_it) {
    assert((it->it_flags & ITEM_SLABBED) == 0);

    do_item_unlink(it);
    return do_item_link(new_it);
}

/*@null@*/

char *do_item_cachedump(const unsigned int slabs_clsid, const unsigned int limit, unsigned int *bytes) {
    unsigned int memlimit = 2 * 1024 * 1024;   /* 2MB max response size */
    char *buffer;
    unsigned int bufcurr;
    item *it;
    unsigned int len;
    unsigned int shown = 0;
    char temp[512];

    if (slabs_clsid > LARGEST_ID) return NULL;
    it = heads[slabs_clsid];

    buffer = malloc((size_t)memlimit);
    if (buffer == 0) return NULL;
    bufcurr = 0;

    while (it != NULL && (limit == 0 || shown < limit)) {
        len = snprintf(temp, sizeof(temp), "ITEM %s [%d b; %lu s]\r\n", ITEM_key(it), it->nbytes - 2, it->exptime + stats.started);
        if (bufcurr + len + 6 > memlimit)  /* 6 is END\r\n\0 */
            break;
        strcpy(buffer + bufcurr, temp);
        bufcurr += len;
        shown++;
        it = it->next;
    }

    memcpy(buffer + bufcurr, "END\r\n", 6);
    bufcurr += 5;

    *bytes = bufcurr;
    return buffer;
}


char *do_item_stats(int *bytes) {
    size_t bufleft = (size_t) LARGEST_ID * 80;
    char *buffer = malloc(bufleft);
    char *bufcurr = buffer;
    rel_time_t now = current_time;
    int i;
    int linelen;

    if (buffer == NULL) {
        return NULL;
    }

    for (i = 0; i < LARGEST_ID; i++) {
        if (tails[i] != NULL) {
            linelen = snprintf(bufcurr, bufleft, "STAT items:%d:number %u\r\nSTAT items:%d:age %u\r\n",
                               i, sizes[i], i, now - tails[i]->time);
            if (linelen + sizeof("END\r\n") < bufleft) {
                bufcurr += linelen;
                bufleft -= linelen;
            }
            else {
                /* The caller didn't allocate enough buffer space. */
                break;
            }
        }
    }
    memcpy(bufcurr, "END\r\n", 6);
    bufcurr += 5;

    *bytes = bufcurr - buffer;
    return buffer;
}

/** dumps out a list of objects of each size, with granularity of 32 bytes */
/*@null@*/

char* do_item_stats_sizes(int *bytes) {
    const int num_buckets = 32768;   /* max 1MB object, divided into 32 bytes size buckets */
    unsigned int *histogram = (unsigned int *)malloc((size_t)num_buckets * sizeof(int));
    char *buf = (char *)malloc(2 * 1024 * 1024); /* 2MB max response size */
    int i;

    if (histogram == 0 || buf == 0) {
        if (histogram) free(histogram);
        if (buf) free(buf);
        return NULL;
    }

    /* build the histogram */
    memset(histogram, 0, (size_t)num_buckets * sizeof(int));
    for (i = 0; i < LARGEST_ID; i++) {
        item *iter = heads[i];
        while (iter) {
            int ntotal = ITEM_ntotal(iter);
            int bucket = ntotal / 32;
            if ((ntotal % 32) != 0) bucket++;
            if (bucket < num_buckets) histogram[bucket]++;
            iter = iter->next;
        }
    }

    /* write the buffer */
    *bytes = 0;
    for (i = 0; i < num_buckets; i++) {
        if (histogram[i] != 0) {
            *bytes += sprintf(&buf[*bytes], "%d %u\r\n", i * 32, histogram[i]);
        }
    }
    *bytes += sprintf(&buf[*bytes], "END\r\n");
    free(histogram);
    return buf;
}

/** returns true if a deleted item's delete-locked-time is over, and it
    should be removed from the namespace */

bool item_delete_lock_over (item *it) {
    assert(it->it_flags & ITEM_DELETED);
    return (current_time >= it->exptime);
}

/** wrapper around assoc_find which does the lazy expiration/deletion logic */

item *do_item_get_notedeleted(const char *key, const size_t nkey, bool *delete_locked) {
    item *it = assoc_find(key, nkey);
    if (delete_locked) *delete_locked = false;
    if (it != NULL && (it->it_flags & ITEM_DELETED)) {
        /* it's flagged as delete-locked.  let's see if that condition
           is past due, and the 5-second delete_timer just hasn't
           gotten to it yet... */
        if (!item_delete_lock_over(it)) {
            if (delete_locked) *delete_locked = true;
            it = NULL;
        }
    }
    if (it != NULL && settings.oldest_live != 0 && settings.oldest_live <= current_time &&
        it->time <= settings.oldest_live) {
        do_item_unlink(it);           /* MTSAFE - cache_lock held */
        it = NULL;
    }
    if (it != NULL && it->exptime != 0 && it->exptime <= current_time) {
        do_item_unlink(it);           /* MTSAFE - cache_lock held */
        it = NULL;
    }

    if (it != NULL) {
        it->refcount++;
        DEBUG_REFCNT(it, '+');
    }
    return it;
}

item *item_get(const char *key, const size_t nkey) {
    return item_get_notedeleted(key, nkey, 0);
}

/** returns an item whether or not it's delete-locked or expired. */

item *do_item_get_nocheck(const char *key, const size_t nkey) {
    item *it = assoc_find(key, nkey);
    if (it) {
        it->refcount++;
        DEBUG_REFCNT(it, '+');
    }
    return it;
}

/* expires items that are more recent than the oldest_live setting. */

void do_item_flush_expired(void) {
    int i;
    item *iter, *next;
    if (settings.oldest_live == 0)
        return;
    for (i = 0; i < LARGEST_ID; i++) {
        /* The LRU is sorted in decreasing time order, and an item's timestamp
         * is never newer than its last access time, so we only need to walk
         * back until we hit an item older than the oldest_live time.
         * The oldest_live checking will auto-expire the remaining items.
         */
        for (iter = heads[i]; iter != NULL; iter = next) {
            if (iter->time >= settings.oldest_live) {
                next = iter->next;
                if ((iter->it_flags & ITEM_SLABBED) == 0) {
                    do_item_unlink(iter);
                }
            } else {
                /* We've hit the first old item. Continue to the next queue. */
                break;
            }
        }
    }
}
