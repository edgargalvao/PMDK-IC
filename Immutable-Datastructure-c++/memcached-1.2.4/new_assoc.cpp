extern "C" {
  #include <memcached.h>
}
//#include <nvm_malloc.h>
#include <immer/map.hpp>
//#include <immer/nvm_utils.hpp>

#include <errno.h>
#include <array>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <time.h>
#include <assert.h>

using MAP_T = immer::map<KEY_T, item*>;
static MAP_T** primary_hashtable;

void assoc_init(void) {
    primary_hashtable = (MAP_T**) nvm_get_id("primary_hashtable");
    if(!primary_hashtable) {
        primary_hashtable = (MAP_T**) nvm_reserve_id("primary_hashtable", sizeof(MAP_T**));
        if(primary_hashtable) {
            *primary_hashtable = new MAP_T();
            nvm_activate_id("primary_hashtable");
			fprintf(stderr, "***************************************************\n");
			fprintf(stderr, "  Allocated hash-table in CURRENT incarnation \n");
			fprintf(stderr, "  primary_hashtable = %p\n", (void*) primary_hashtable);
			fprintf(stderr, "***************************************************\n");
        } else {
            fprintf(stderr, "Failed to init hashtable.\n");
            exit(EXIT_FAILURE);
        }
    } else {
			fprintf(stderr, "***************************************************\n");
			fprintf(stderr, "  Using hash-table from PREVIOUS incarnation \n");
			fprintf(stderr, "  primary_hashtable = %p\n", (void*) primary_hashtable);
			fprintf(stderr, "***************************************************\n");
    }
}

KEY_T convertKey(const char *key, const size_t nkey) {
    char key_buf[16];
    assert(nkey == 16);
    memcpy(&key_buf, key, nkey);
    return atol(key_buf);
}

item* assoc_find(KEY_T key) {
    item* const* found_item = (*primary_hashtable)->find(key);
    if (found_item == nullptr)
        return 0;
    return *found_item;
}

item* assoc_find(const char *key, const size_t nkey) {
    return assoc_find(convertKey(key, nkey));
}

/* Note: this isn't an assoc_update.  The key must not already exist to call this */
int assoc_insert(item *it) {
    assert(it->nkey == 16);
    auto key = convertKey(ITEM_key(it), it->nkey);
    assert(assoc_find(key) == 0);  // shouldn't have duplicately named things defined
    auto old_table = *primary_hashtable;
    *primary_hashtable = (*primary_hashtable)->set_ptr(key, it);
    NVM_SFENCE();
    delete old_table;
    return 1;
}

void assoc_delete(const char *key, const size_t nkey) {
    // We don't check to see if item is present as callers dont call
    // delete if they dont find the item!
    assert(nkey == 16);
    auto uint_key = convertKey(key, nkey);
    auto old_table = *primary_hashtable;
    *primary_hashtable = (*primary_hashtable)->erase_ptr(uint_key);
    NVM_SFENCE();
    delete old_table;
}
