/* -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Slabs memory allocation, based on powers-of-N. Slabs are up to 1MB in size
 * and are divided into chunks. The chunk sizes start off at the size of the
 * "item" structure plus space for a small key and value. They increase by
 * a multiplier factor from there, up to half the maximum slab size. The last
 * slab size is always 1MB, since that's the maximum item size allowed by the
 * memcached protocol.
 *
 * $Id: slabs.c 591 2007-07-09 14:28:54Z plindner $
 */
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <memcached.h>

#define POWER_SMALLEST 1
#define POWER_LARGEST  200
#define POWER_BLOCK 1048576 // slab size = 1MB
#define CHUNK_ALIGN_BYTES (sizeof(void *))
//#define DONT_PREALLOC_SLABS

/* powers-of-N allocation structures */

typedef struct {
	// All pointers in this struct are persistent ?!
    unsigned int size;      /* sizes of items */
    unsigned int perslab;   /* how many items per slab */

    void **slots;           /* list of item ptrs */
    unsigned int sl_total;  /* size of previous array */
    unsigned int sl_curr;   /* first free slot */

    void *end_page_ptr;         /* pointer to next free item at end of page, or 0 */
    unsigned int end_page_free; /* number of items remaining at end of last alloced page */

    unsigned int slabs;     /* how many slabs were allocated for this class */

    void **slab_list;       /* array of slab pointers */
    unsigned int list_size; /* size of prev array */

    unsigned int killing;  /* index+1 of dying slab, or zero if none */
} slabclass_t;

static slabclass_t slabclass[POWER_LARGEST + 1];
static size_t mem_limit = 0;
static size_t mem_malloced = 0;
static int power_largest;

/*
 * Forward Declarations
 */
int do_slabs_newslab(const unsigned int id);

#ifndef DONT_PREALLOC_SLABS
/* Preallocate as many slab pages as possible (called from slabs_init)
   on start-up, so users don't get confused out-of-memory errors when
   they do have free (in-slab) space, but no space to make new slabs.
   if maxslabs is 18 (POWER_LARGEST - POWER_SMALLEST + 1), then all
   slab types can be made.  if max memory is less than 18 MB, only the
   smaller ones will be made.  */
static void slabs_preallocate (const unsigned int maxslabs);
#endif

/*
 * Figures out which slab class (chunk size) is required to store an item of
 * a given size.
 *
 * Given object size, return id to use when allocating/freeing memory for object
 * 0 means error: can't store such a large object
 */


unsigned int slabs_clsid(const size_t size) {
    int res = POWER_SMALLEST;

    if (size == 0)
        return 0;
    while (size > slabclass[res].size)
        if (res++ == power_largest)     /* won't fit in the biggest slab */
            return 0;
    return res;
}

/**
 * Determines the chunk sizes and initializes the slab class descriptors
 * accordingly.
 */
void slabs_init(const size_t limit, const double factor) {
    int i = POWER_SMALLEST - 1;
    unsigned int size = sizeof(item) + settings.chunk_size;

    /* Factor of 2.0 means use the default memcached behavior */
	// If factor is 2 then the chunk size defaults to 128, Sanketh
    if (factor == 2.0 && size < 128)
        size = 128;

    mem_limit = limit;
    memset(slabclass, 0, sizeof(slabclass));

    while (++i < POWER_LARGEST && size <= POWER_BLOCK / 2) {
        /* Make sure items are always n-byte aligned */
        if (size % CHUNK_ALIGN_BYTES)
            size += CHUNK_ALIGN_BYTES - (size % CHUNK_ALIGN_BYTES);

        slabclass[i].size = size;
        slabclass[i].perslab = POWER_BLOCK / slabclass[i].size;
        size *= factor;
        if (settings.verbose > 1) {
            fprintf(stderr, "slab class %3d: chunk size %6u perslab %5u\n",
                    i, slabclass[i].size, slabclass[i].perslab);
        }
    }

    power_largest = i;
    slabclass[power_largest].size = POWER_BLOCK;
    slabclass[power_largest].perslab = 1;

    /* for the test suite:  faking of how much we've already malloc'd */
    {
        char *t_initial_malloc = getenv("T_MEMD_INITIAL_MALLOC");
        if (t_initial_malloc) {
            // fprintf(stderr, "T_MEMD_INITIAL_MALLOC\n");
            mem_malloced = (size_t)atol(t_initial_malloc);
        }

    }

#ifndef DONT_PREALLOC_SLABS
    {
        char *pre_alloc = getenv("T_MEMD_SLABS_ALLOC");

        if (pre_alloc == NULL || atoi(pre_alloc) != 0) {
            fprintf(stderr, "T_MEMD_SLABS_MALLOC\n");
            slabs_preallocate(power_largest);
        }
    }
#endif
}

#ifndef DONT_PREALLOC_SLABS
static void slabs_preallocate (const unsigned int maxslabs) {
    int i;
    unsigned int prealloc = 0;

    /* pre-allocate a 1MB slab in every size class so people don't get
       confused by non-intuitive "SERVER_ERROR out of memory"
       messages.  this is the most common question on the mailing
       list.  if you really don't want this, you can rebuild without
       these three lines.  */

    for (i = POWER_SMALLEST; i <= POWER_LARGEST; i++) {
        if (++prealloc > maxslabs)
            return;
        do_slabs_newslab(i);
    }

}
#endif


int grow_slab_list (const unsigned int id) {
    slabclass_t *p = &slabclass[id];
    if (p->slabs == p->list_size) {
        size_t new_size =  (p->list_size != 0) ? p->list_size * 2 : 16;
        void *new_list = realloc(p->slab_list, new_size * sizeof(void *));
        if (new_list == 0) return 0;
        p->list_size = new_size;
        p->slab_list = new_list;
    }
    return 1;
}


int do_slabs_newslab(const unsigned int id) {
    slabclass_t *p = &slabclass[id];
#ifdef ALLOW_SLABS_REASSIGN
    int len = POWER_BLOCK;
#else
    int len = p->size * p->perslab;
#endif
    char *ptr;

    if (mem_limit && mem_malloced + len > mem_limit && p->slabs > 0)
        return 0;

    if (grow_slab_list(id) == 0) return 0;

	/* Sanketh */
    // fprintf(stderr, "%s : PMALLOC\n", __func__);
	/* So you never preallocate the entire
	   the entire pool of slabs. You always
	   do it incrementally.
	 */
    ptr = (char*) nvm_reserve((size_t)len);
    if (ptr == 0) return 0;
	// This eats up the Tx write-set, then let pmalloc zero the memory
    memset(ptr, 0, (size_t)len);
    p->end_page_ptr = ptr;
    p->end_page_free = p->perslab;

    p->slab_list[p->slabs++] = ptr;
    mem_malloced += len;
    return 1;
}

/*@null@*/

void *do_slabs_alloc(const size_t size) {
    slabclass_t *p;

	// getting the slab id here 
    unsigned int id = slabs_clsid(size);

	//iIf the item is out of the range of sizes
	// supported by memcached, return null !!!
    if (id < POWER_SMALLEST || id > power_largest)
        return NULL;

	// getting the pointer to the array of slabs 
	// in the slab class id
	// all slabs are persistent !!! 
    p = &slabclass[id];

    assert(p->sl_curr == 0 || ((item *)p->slots[p->sl_curr - 1])->slabs_clsid == 0);

// Have to disable this to prevent 
// memcached from asking more slabs !!
#ifdef USE_SYSTEM_MALLOC
	// This code is wrong !
	// You can't just return a alloc'ed 
	// chunk of memory without putting it 
	// in a slab first or taking it out
	// of a prealloc'ed slab !!  Sanketh
    if (mem_limit && mem_malloced + size > mem_limit)
        return 0;
	/* How do you know the malloc will succeed ?? */
//	fprintf(stderr, "Using system malloc (a)\n");
    mem_malloced += size;
	/* Sanketh  here !!!!*/
    return nvm_reserve(size);
	// This code never runs anyways !!
#endif

// LRU is not performed here !!
// It is performed in the caller of this func 
    /* fail unless we have space at the end of a recently allocated page,
       we have something on our freelist, or we could allocate a new page */
	/* get something of the free list or allocate a new page */

    if (! (p->end_page_ptr != 0 || p->sl_curr != 0 || do_slabs_newslab(id) != 0))
        return 0;

    /* return off our freelist, if we have one */
    if (p->sl_curr != 0)
        return p->slots[--p->sl_curr];

    /* if we recently allocated a whole page, return from that */
    if (p->end_page_ptr) {
        void *ptr = p->end_page_ptr;
        if (--p->end_page_free != 0) {
            p->end_page_ptr += p->size;
        } else {
            p->end_page_ptr = 0;
        }
        return ptr;
    }

    return NULL;  /* shouldn't ever get here */
}


void do_slabs_free(void *ptr, const size_t size) {
    unsigned char id = slabs_clsid(size);
    slabclass_t *p;

    assert(((item *)ptr)->slabs_clsid == 0);
    assert(id >= POWER_SMALLEST && id <= power_largest);
    if (id < POWER_SMALLEST || id > power_largest)
        return;

    p = &slabclass[id];

#ifdef USE_SYSTEM_MALLOC
    mem_malloced -= size;
    pfree(ptr);
    return;
#endif

    if (p->sl_curr == p->sl_total) { /* need more space on the free list */
        int new_size = (p->sl_total != 0) ? p->sl_total * 2 : 16;  /* 16 is arbitrary */
        void **new_slots = realloc(p->slots, new_size * sizeof(void *));
        if (new_slots == 0)
            return;
        p->slots = new_slots;
        p->sl_total = new_size;
    }
    p->slots[p->sl_curr++] = ptr;
    return;
}

/*@null@*/

char* do_slabs_stats(int *buflen) {
    int i, total;
    char *buf = (char *)malloc(power_largest * 200 + 100);
    char *bufcurr = buf;

    *buflen = 0;
    if (buf == NULL) return NULL;

    total = 0;
    for(i = POWER_SMALLEST; i <= power_largest; i++) {
        slabclass_t *p = &slabclass[i];
        if (p->slabs != 0) {
            unsigned int perslab, slabs;

            slabs = p->slabs;
            perslab = p->perslab;
	 {
	            bufcurr += sprintf(bufcurr, "STAT %d:chunk_size %u\r\n", i, p->size);
    	        bufcurr += sprintf(bufcurr, "STAT %d:chunks_per_page %u\r\n", i, perslab);
        	    bufcurr += sprintf(bufcurr, "STAT %d:total_pages %u\r\n", i, slabs);
            	bufcurr += sprintf(bufcurr, "STAT %d:total_chunks %u\r\n", i, slabs*perslab);
	            bufcurr += sprintf(bufcurr, "STAT %d:used_chunks %u\r\n", i, slabs*perslab - p->sl_curr);
    	        bufcurr += sprintf(bufcurr, "STAT %d:free_chunks %u\r\n", i, p->sl_curr);
        	    bufcurr += sprintf(bufcurr, "STAT %d:free_chunks_end %u\r\n", i, p->end_page_free);
			}	
            total++;
        }
    }
	
	{
	    bufcurr += sprintf(bufcurr, "STAT active_slabs %d\r\nSTAT total_malloced %llu\r\n", total, (unsigned long long)mem_malloced);
    	bufcurr += sprintf(bufcurr, "END\r\n");
	}
	
    *buflen = bufcurr - buf;
    return buf;
}

#ifdef ALLOW_SLABS_REASSIGN
/* Blows away all the items in a slab class and moves its slabs to another
   class. This is only used by the "slabs reassign" command, for manual tweaking
   of memory allocation. It's disabled by default since it requires that all
   slabs be the same size (which can waste space for chunk size mantissas of
   other than 2.0).
   1 = success
   0 = fail
   -1 = tried. busy. send again shortly. */
int do_slabs_reassign(unsigned char srcid, unsigned char dstid) {
    void *slab, *slab_end;
    slabclass_t *p, *dp;
    void *iter;
    bool was_busy = false;

    if (srcid < POWER_SMALLEST || srcid > power_largest ||
        dstid < POWER_SMALLEST || dstid > power_largest)
        return 0;

    p = &slabclass[srcid];
    dp = &slabclass[dstid];

    /* fail if src still populating, or no slab to give up in src */
    if (p->end_page_ptr || ! p->slabs)
        return 0;

    /* fail if dst is still growing or we can't make room to hold its new one */
    if (dp->end_page_ptr || ! grow_slab_list(dstid))
        return 0;

    if (p->killing == 0) p->killing = 1;

    slab = p->slab_list[p->killing - 1];
    slab_end = (char*)slab + POWER_BLOCK;

    for (iter = slab; iter < slab_end; (char*)iter += p->size) {
        item *it = (item *)iter;
        if (it->slabs_clsid) {
            if (it->refcount) was_busy = true;
            do_item_unlink(it);
        }
    }

    /* go through free list and discard items that are no longer part of this slab */
    {
        int fi;
        for (fi = p->sl_curr - 1; fi >= 0; fi--) {
            if (p->slots[fi] >= slab && p->slots[fi] < slab_end) {
                p->sl_curr--;
                if (p->sl_curr > fi) p->slots[fi] = p->slots[p->sl_curr];
            }
        }
    }

    if (was_busy) return -1;

    /* if good, now move it to the dst slab class */
    p->slab_list[p->killing - 1] = p->slab_list[p->slabs - 1];
    p->slabs--;
    p->killing = 0;
    dp->slab_list[dp->slabs++] = slab;
    dp->end_page_ptr = slab;
    dp->end_page_free = dp->perslab;
    /* this isn't too critical, but other parts of the code do asserts to
       make sure this field is always 0.  */
    for (iter = slab; iter < slab_end; (char*)iter += dp->size) {
        ((item *)iter)->slabs_clsid = 0;
    }
    return 1;
}
#endif
