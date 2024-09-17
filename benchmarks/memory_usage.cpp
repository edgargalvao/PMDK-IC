/*
 * Copyright (c) 2017 The National University of Singapore.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <chrono> 
#include <thread> 
#include <random>
#include <cassert>
#include <climits>
#include <cstdio> // remove (file)
#include <unistd.h> // getopt
#include <sys/stat.h> // stat
#include <mutex> // For std::unique_lock
#include <shared_mutex>
#include <x86intrin.h>

#define IMMER_USE_NVM true

#include <immer/map.hpp>
#include <immer/set.hpp>
#include <immer/concurrent_map.hpp>
#include <immer/concurrent_vector.hpp>
#include <immer/set.hpp>
#include <immer/array.hpp>
#include <immer/queue.hpp>
#include <immer/detail/list/list.hpp>
#include <immer/array_transient.hpp>
#include <immer/vector.hpp>
#include <immer/vector_transient.hpp>
#include <include/libpmemobj.h>
#include <include/libpmemobj/tx.h>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>
#include <libpmemobj++/experimental/vector.hpp>
#include <libpmemobj++/experimental/array.hpp>

#include "concurrent_array.hpp"

extern "C"
{
#ifndef PMDK_NO_TX
#include <examples/libpmemobj/hashmap/hashmap_tx.h>
#include <examples/libpmemobj/hashmap/set_tx.h>
#include <examples/libpmemobj/map/map.h>
#include <examples/libpmemobj/linkedlist/pmemobj_list.h>
#else
#include <examples/libpmemobj/hashmap/hashmap_notx.h>
#include <examples/libpmemobj/hashmap/set_notx.h>
#include <examples/libpmemobj/map/map.h>
#include <examples/libpmemobj/linkedlist/pmemobj_list_notx.h>
#endif // PMDK_NO_TX
#ifdef VALGRIND
#include <lackey.h>
#endif // VALGRIND
}

using namespace std;

//common stuff
constexpr auto maxThreads = 8;
constexpr auto poolSize = 16*1024*1024*1024UL;
using bench_type1 = std::function<void(bool, int)>;
using bench_type2 = std::function<void(int, int)>;
using check_type = std::function<void(void)>;

// IMMER stuff
using keyType = uint64_t;
constexpr auto keySize = sizeof(keyType);
constexpr auto valueSize = 32;
constexpr auto arraySize = 100000; 

using valueType = std::array<char, valueSize>;
using map_type = immer::map<keyType, valueType>;
using set_type = immer::set<keyType>;
using concurrent_map_type = immer::concurrent_map<keyType, valueType>;
using vector_type = immer::vector<uint64_t>;
using concurrent_vector_type = immer::concurrent_vector<uint64_t>;
using array_type = immer::array<uint64_t>;
using queue_type = immer::queue<uint64_t>;
using list_type = immer::list<uint64_t>;
using pmdk_vector_type = pmem::obj::experimental::vector<uint64_t>;
using pmdk_array_type = pmem::obj::experimental::array<uint64_t, arraySize>;

// PMDK stuff
#define OBJ_TYPE_NUM 1

POBJ_LAYOUT_BEGIN(layout_map);
POBJ_LAYOUT_ROOT(layout_map, struct root);
POBJ_LAYOUT_END(layout_map);

struct root {
	TOID(struct map) map;
};

static TOID(struct root) root;

POBJ_LAYOUT_BEGIN(layout_set);
POBJ_LAYOUT_ROOT(layout_set, struct Set_root);
POBJ_LAYOUT_END(layout_set);

#ifndef PMDK_NO_TX
struct Set_root {
	TOID(struct set_tx) set;
};
#else
struct Set_root {
	TOID(struct set_notx) set;
};
#endif // PMDK_NO_TX

static TOID(struct Set_root) set_root;

POBJ_LAYOUT_BEGIN(layout_array);
POBJ_LAYOUT_TOID(layout_array, int);
POBJ_LAYOUT_END(layout_array);

POBJ_LAYOUT_BEGIN(queue);
POBJ_LAYOUT_ROOT(queue, struct fifo_root);
POBJ_LAYOUT_TOID(queue, struct tqnode);
POBJ_LAYOUT_END(queue);

POBJ_TAILQ_HEAD(tqueuehead, struct tqnode);

struct fifo_root {
	struct tqueuehead head;
};

struct tqnode {
	uint64_t data;
	POBJ_TAILQ_ENTRY(struct tqnode) tnd;
};

POBJ_LAYOUT_BEGIN(list);
POBJ_LAYOUT_ROOT(list, struct list_root);
POBJ_LAYOUT_TOID(list, struct tlnode);
POBJ_LAYOUT_END(list);

POBJ_SLIST_HEAD(tlisthead, struct tlnode);

struct list_root {
	struct tlisthead head;
};

struct tlnode {
	uint64_t data;
	POBJ_SLIST_ENTRY(struct tlnode) tnd;
};

struct vector_root {
    pmem::obj::persistent_ptr<pmdk_vector_type> v_ptr;
};

struct array_root {
    pmem::obj::persistent_ptr<pmdk_array_type> v_ptr;
};

PMEMobjpool *pop;
pmem::obj::pool<vector_root>* vector_pool_ptr;
pmem::obj::persistent_ptr<vector_root> r;

pmem::obj::pool<array_root>* array_pool_ptr;
pmem::obj::persistent_ptr<array_root> a_r;

// Global Definitions
//
// Datastructures defined here to ensure they stay in scope for the benchmark lambda
map_type* immer_map;
set_type* immer_set;
vector_type* immer_vector;
concurrent_vector_type* concurrent_immer_vector;
concurrent_map_type* concurrent_immer_map;
array_type* immer_array;
queue_type* immer_queue;
list_type* immer_list;
pmdk_array_type* exp_pmdk_array;
pmdk_vector_type* pmdk_vector;


int *keys;
bool *writes;
valueType value;
PMEMoid value_ptr; 
PMEMoid pmdk_array;
int *array_direct;
#ifndef PMDK_NO_TX
TOID(struct hashmap_tx) *map; 
TOID(struct set_tx) *set; 
#else
TOID(struct hashmap_notx) *map; 
TOID(struct set_notx) *set; 
#endif // PMDK_NO_TX
concurrent_array *tester;
struct tqueuehead *tqhead;
TOID(struct tqnode) qnode;
struct tlisthead *tlhead;
TOID(struct tlnode) lnode;

struct ConfigOptions {
    string datastructure;
    string backing_file;
    string impl;
    // TODO: Create some sequential/random/zipfian key/value patterns.
    string pattern = "seq";
    int threads = 1;
    int num_ops = 0;
    int ops_per_thread = 0;
    int num_elems = 0;
    int percent_writes = 100;
    int seed = 0;
    bool enter_roi= true; // If false, exit before ROI to get baseline result
    bool Validate () {
        return (!backing_file.empty() && !datastructure.empty()
             && !impl.empty() && (threads >0) && (num_ops > 0)
             && (num_elems > 0));

    }
};

bool FileExists (const char *file) {
    struct stat buffer;
    return (stat (file, &buffer) == 0);
}

bool DeleteFile (const char *file) {
    remove(file);
    // Return false if file still exists!
    return !FileExists(file); 
}

bool SetUpBackingFile(const char *file) {
    if (FileExists(file)) {
        cout << "Backing file already exists, will erase." << endl;
        if (!DeleteFile(file)) {
            return false;
        }
    }
    return true;
    // File will be created by either nvm_malloc or PMDK.
    // return CreateFile(file);
}

void PrintUsage () {
    cerr << "Error!/ Correct Usage: ./bench"
        " -i implementation -d datastructure -e num_elems"
        " -n num_ops -f backing-file -t threads [-r]\n" << endl;
}

void GenerateZeroValue(valueType& value) {
    std::string str_value;
    str_value.append(valueSize, '0');
    std::copy(str_value.begin(), str_value.begin()+valueSize, value.begin());
}
void GenerateValue(valueType& value) {
    std::string str_value;
    str_value.append(valueSize, 'X');
    std::copy(str_value.begin(), str_value.begin()+valueSize, value.begin());
}

void PopulateKeysAndOps(const ConfigOptions& options, int *keys, bool *writes) {
    std::random_device device;
    int random_seed = options.seed;
    if (random_seed == 0)
        random_seed = device();
    std::mt19937 generator(random_seed);
    std::uniform_int_distribution<int> 
        distribution(0, options.num_ops);

    if (options.pattern == "seq" || options.pattern == "rand") {
        for (int i = 0; i < options.num_ops; i++) {
            keys[i] = i;
        }
        if (options.pattern == "rand")
            std::shuffle(keys, keys+options.num_ops, generator);
    } else {
        cerr << "Unimplemented pattern:" << options.pattern << endl;
        assert(0);
    }

    // Populate ops with reads/writes as per write_percent.
    std::uniform_int_distribution<int> percent_distribution(1, 100);
    for (int i = 0; i < options.num_ops; i++) {
        if (percent_distribution(generator) <= options.percent_writes)
            writes[i] = true;
        else 
            writes[i] = false;
    }

    // Debugging feature
    if (options.num_ops < 100) {
        cout << "Keys:" << endl;
        for (int i = 0; i < options.num_ops; i++) {
            cout << keys[i] << " ";
        }
        cout << endl;
        cout << "Operations:" << endl;
        for (int i = 0; i < options.num_ops; i++) {
            cout << (writes[i] ? "w" : "r") << " ";
        }
        cout << endl;
    }
}

void ThreadWork(int thread_id, int ops_per_thread,
        bench_type1 benchOne, bench_type2 benchTwo) {

    if (!benchOne && benchTwo) {
        cout << "Oooh! Running an advanced benchmark" << endl;
    }
    uint64_t thread_offset = thread_id*ops_per_thread;
    for (uint64_t i = 0; i < ops_per_thread; i++) {
        auto offset = thread_offset+i;
        auto key = keys[offset];
        auto write = writes[offset];
#ifdef VALGRIND
        LACKEY_OPERATION_START;
#endif
        if (benchOne)
            benchOne(write, key);
        else {
            auto key2 = keys[offset+1];
            i++;
            benchTwo(key, key2);
        }
        //nvm_report_mem();
#ifdef VALGRIND
        LACKEY_OPERATION_END;
#endif
    }
}

void BeginValgrindInstrumentation() {
#ifdef VALGRIND
    // Stop and Start flushes all counters
    LACKEY_STOP_INSTRUMENTATION;
    LACKEY_START_INSTRUMENTATION;
#endif
}

// TODO: Add checker for all benchmarks
void RunBenchmark (const ConfigOptions& options) {

    std::thread workers[maxThreads];
    std::chrono::time_point<std::chrono::high_resolution_clock> start, stop;
    bench_type1 benchOne = NULL;
    bench_type2 benchTwo = NULL;
    std::function<void(void)> checker;
    keys = new int [options.num_ops];
    writes = new bool [options.num_ops];

    PopulateKeysAndOps(options, keys, writes);

    if (options.impl == "immer") {
        nvm_initialize(options.backing_file.c_str(), 0);
        if (options.datastructure == "map") {
            immer_map = new map_type(); 
            GenerateValue(value);
	    for (int i = 0; i < options.num_elems; i++) {
		    auto *old_map = immer_map;
		    immer_map = immer_map->set_ptr(i, value);
		    _mm_sfence();
		    delete old_map;
	    }
            if (options.threads == 1) {

                benchOne = [&](bool write, int key) {
                    if (write) {
                        auto* old_map = immer_map;
			// We want to insert new keys.
			// [0..num_elems] already in the map
                        immer_map = immer_map->set_ptr(options.num_elems+key, value);
#ifndef IMMER_DISABLE_FLUSHING
                        _mm_sfence();
#endif
                        delete old_map;
                    } else {
                        volatile const valueType* val = immer_map->find(key);
                        assert(*(const valueType*)val == value);
                    }
                };
            } else {
                GenerateValue(value);
                concurrent_immer_map = new concurrent_map_type(&immer_map);
                benchOne = [&](bool write, int key) {
                    // Concurrent_immer_map is neither immutable nor
                    // persistent, but just a wrapper around an immutable
                    // and persistent map.
                    if (write) {
                        concurrent_immer_map->set(options.num_elems+key, value);
                    } else {
                        volatile auto* val = immer_map->find(key);
                    }
                };
            }
            checker = [&]() {
                assert(immer_map->size() == options.num_ops);
            };
        } else if (options.datastructure == "set") {
            immer_set = new set_type(); 
            if (options.threads == 1) {
				for (int i = 0; i < options.num_elems; i++) {
					auto *old_set = immer_set;
					immer_set = immer_set->insert_ptr(i);
					_mm_sfence();
					delete old_set;
				}
                benchOne = [&](bool write, int key) {
                    if (write) {
                        auto* old_set = immer_set;
                        immer_set = immer_set->insert_ptr(options.num_elems+key);
#ifndef IMMER_DISABLE_FLUSHING
                        _mm_sfence();
#endif
                        delete old_set;
                    } else {
                        volatile auto val = immer_set->count(key);
                    }
                };
            } else {
            }
            checker = [&]() {
                assert(immer_set->size() == options.num_ops);
            };
        } else if (options.datastructure == "vector") {
            immer_vector = new vector_type(options.num_ops, 0);
            if (options.threads == 1) {
                benchOne = [&](bool write, int key) {
                    if (write) { 
                        auto* old_vector = immer_vector;
                        immer_vector = immer_vector->set_ptr(key, key);
#ifndef IMMER_DISABLE_FLUSHING
                        _mm_sfence();
#endif
                        delete old_vector;
                    } else {
                        volatile auto val = (*immer_vector)[key];
                    }
                };
            } else {
                concurrent_immer_vector =
                    new concurrent_vector_type(&immer_vector);
                benchOne = [&](bool write, int key) {
                    if (write) {
                        concurrent_immer_vector->set(key, key);
                    } else {
                        cerr << "Reads not supported for vector yet";
                        assert(0);
                    }
                };
            }
            checker = [&]() {
                //assert(immer_vector->size() == options.num_ops);
                // TODO: Ensure appropriate elements are set to index number.
            };
        } else if (options.datastructure == "vector-swaps") {
            immer_vector = new vector_type(options.num_elems, 1);
            benchTwo = [&](int key1, int key2) {
                auto val1 = (*immer_vector)[key1];
                auto val2 = (*immer_vector)[key2];
                auto* old_vector = immer_vector;
                //immer_vector = immer_vector->set_ptr(key1, val2);
                //immer_vector = immer_vector->set_ptr(key2, val1);
                auto* transient_vector = immer_vector->transient_ptr();
                transient_vector->set(key1, val2);
                transient_vector->set(key2, val1);
                immer_vector = reinterpret_cast<vector_type*>(transient_vector);
                //immer_vector = transient_vector->persistent_ptr();
#ifndef IMMER_DISABLE_FLUSHING
                _mm_sfence();
#endif
                delete old_vector;
            };
        } else if (options.datastructure == "array") {
            assert(options.threads == 1);
            immer_array = new array_type(options.num_ops, 0); 
            benchOne = [&](bool write, int key) {
                if(write) {
                    auto* old_array = immer_array;
                    immer_array = immer_array->set_ptr(key, key);
#ifndef IMMER_DISABLE_FLUSHING
                    _mm_sfence();
#endif
                    delete old_array;
                } else {
                    volatile auto val = immer_array->at(key);
                }
            };
        } else if (options.datastructure == "array-swaps") {
            assert(options.threads == 1);
            immer_array = new array_type(options.num_elems, 0); 
            benchTwo = [&](int key1, int key2) {
                auto* old_array = immer_array;
                auto* transient_array = immer_array->transient_ptr();
                auto val1 = immer_array->at(key1);
                auto val2 = immer_array->at(key2);
                transient_array->set(key1, val2);
                transient_array->set(key2, val1);
                immer_array = transient_array->persistent_ptr();
#ifndef IMMER_DISABLE_FLUSHING
                _mm_sfence();
#endif
                delete old_array;
            };
        } else if (options.datastructure == "queue") {
            assert(options.threads == 1);
            immer_queue = new queue_type(); 
		// Need to handle cases when too many pops happen before pushes.
	    for (int i = 0; i < options.num_elems; i++) {
		auto* old_queue = immer_queue;
		immer_queue = immer_queue->push_back_ptr(i);
		delete old_queue;
	    } 
            benchOne = [&](bool write, int key) {
                auto* old_queue = immer_queue;
                if(write) {
                    immer_queue = immer_queue->push_back_ptr(key);
                } else {
                    immer_queue = immer_queue->pop_front_ptr();
                }
#ifndef IMMER_DISABLE_FLUSHING
                _mm_sfence();
#endif
                delete old_queue;
            };
        } else if (options.datastructure == "queue2") {
            assert(options.threads == 1);
            immer_queue = new queue_type(); 
	    // Need to handle cases when too many pops happen before pushes.
	    // +1 because we pop an element at the beginning
	    for (int i = 0; i < options.num_elems; i++) {
		auto* old_queue = immer_queue;
		immer_queue = immer_queue->push_back_ptr(i);
		delete old_queue;
	    } 
	    immer_queue = immer_queue->pop_front_ptr();
            benchOne = [&](bool write, int key) {
                auto* old_queue = immer_queue;
                if(write) {
                    immer_queue = immer_queue->push_back_ptr(key);
                } else {
                    immer_queue = immer_queue->pop_front_ptr();
                }
#ifndef IMMER_DISABLE_FLUSHING
                _mm_sfence();
#endif
                delete old_queue;
            };
        } else if (options.datastructure == "stack") {
            assert(options.threads == 1);
            immer_list = new list_type();
	    for (int i = 0; i < options.num_elems; i++) {
		auto* old_list = immer_list;
		immer_list = immer_list->prepended_ptr(i);
		delete old_list;
	    } 
            benchOne = [&](bool write, int key) {
                auto* old_list = immer_list;
                if (write) {
                    immer_list = immer_list->prepended_ptr(key);
                } else {
                    immer_list = immer_list->popped_front_ptr();
                }
#ifndef IMMER_DISABLE_FLUSHING
                _mm_sfence();
#endif
                delete old_list;
            };
        } else {
            cerr << "Unsupported datastructure:" << options.datastructure
                << endl;
            return;
        }
    } else if (options.impl == "pmdk") {
        assert(options.threads==1);
        if (options.datastructure == "map") {
            if ((pop = pmemobj_create(options.backing_file.c_str(),
                            POBJ_LAYOUT_NAME(layout_map),
                            poolSize, 0666)) == NULL) {
                cerr << "failed to create pool:" << pmemobj_errormsg() << endl;
                return;
            }
            root = POBJ_ROOT(pop, struct root);
#ifndef PMDK_NO_TX
            map = (TOID(struct hashmap_tx) *) &D_RW(root)->map;
            if (hm_tx_create(pop, map, NULL)) {
                perror("map_new");
                return;
            }
#else
            map = (TOID(struct hashmap_notx) *) &D_RW(root)->map;
            if (hm_notx_create(pop, map, NULL)) {
                perror("map_new");
                return;
            }
#endif
	    for (int i = 0; i < options.num_elems; i++) {
		    pmemobj_zalloc(pop, &value_ptr, sizeof(uint64_t), OBJ_TYPE_NUM);
#ifndef PMDK_NO_TX
		    hm_tx_insert(pop, *map, i, value_ptr); 
#else
		    hm_notx_insert(pop, *map, i, value_ptr); 
#endif
	    }
            benchOne = [&](bool write, int key) {
                if (write) {
                    // We allocate explicitly here as insert
                    // needs pointer to allocated value.
                    // Zalloc zeroes out location, which acts as dummy of 
                    // actually updating location with real value.
                    // We want to actually use alloc and then write in real
                    // value, but hard to do as it takes parameter contructor.
                    pmemobj_zalloc(pop, &value_ptr, sizeof(uint64_t), OBJ_TYPE_NUM);
		    // We want to insert new keys.
		    // [0..num_elems] already in the map
#ifndef PMDK_NO_TX
                    hm_tx_insert(pop, *map, options.num_elems+key, value_ptr); 
#else
                    hm_notx_insert(pop, *map, options.num_elems+key, value_ptr); 
#endif
                } else {
#ifndef PMDK_NO_TX
                    volatile auto val = hm_tx_get(pop, *map, key);
#else
                    volatile auto val = hm_notx_get(pop, *map, key);
#endif
                }
            };
        } else if (options.datastructure == "set") {
            if ((pop = pmemobj_create(options.backing_file.c_str(),
                            POBJ_LAYOUT_NAME(layout_set),
                            poolSize, 0666)) == NULL) {
                cerr << "failed to create pool:" << pmemobj_errormsg() << endl;
                return;
            }
            set_root = POBJ_ROOT(pop, struct Set_root);
#ifndef PMDK_NO_TX
            set = (TOID(struct set_tx) *) &D_RW(set_root)->set;
            if (set_tx_create(pop, set, NULL)) {
                perror("set_new");
                return;
            } 
#else
            set = (TOID(struct set_notx) *) &D_RW(set_root)->set;
            if (set_notx_create(pop, set, NULL)) {
                perror("set_new");
                return;
            }
#endif
	    for (int i = 0; i < options.num_elems; i++) {
#ifndef PMDK_NO_TX
		    set_tx_insert(pop, *set, i); 
#else
		    set_notx_insert(pop, *set, i); 
#endif
	    }
	    benchOne = [&](bool write, int key) {
#ifndef PMDK_NO_TX
                if (write) {
                    set_tx_insert(pop, *set, options.num_elems+key); 
                } else {
                    volatile auto val = set_tx_lookup(pop, *set, key);
                }
#else
                if (write) {
                    set_notx_insert(pop, *set, options.num_elems+key); 
                } else {
                    volatile auto val = set_notx_lookup(pop, *set, key);
                }
#endif
            };
        } else if (options.datastructure == "vector") {
            auto vector_pool = pmem::obj::pool<vector_root>::create(options.backing_file.c_str(), "vector",
                        poolSize, S_IWUSR|S_IRUSR);
            vector_pool_ptr = &vector_pool;
            r = vector_pool_ptr->root();
            pmem::obj::transaction::run(*vector_pool_ptr, [&] {
                r->v_ptr = pmem::obj::make_persistent<pmdk_vector_type>(options.num_ops, 0);
            });
            benchOne = [&](bool write, int key) {
#ifndef PMDK_NO_TX
                if (write) { 
                    pmem::obj::transaction::run(*vector_pool_ptr, [&] {
                            r->v_ptr->at(key) = key;
                    });
                } else {
                    pmem::obj::transaction::run(*vector_pool_ptr, [&] {
                        volatile auto val = r->v_ptr->const_at(key);
                    });
                }
#else 
                if (write) { 
                            r->v_ptr->at(key) = key;
                } else {
                        volatile auto val = r->v_ptr->const_at(key);
                }
#endif
            };
        } else if (options.datastructure == "vector-swaps") {
            auto vector_pool = pmem::obj::pool<vector_root>::create(options.backing_file.c_str(), "vector",
                        poolSize, S_IWUSR|S_IRUSR);
            vector_pool_ptr = &vector_pool;
            r = vector_pool_ptr->root();
            pmem::obj::transaction::run(*vector_pool_ptr, [&] {
                r->v_ptr = pmem::obj::make_persistent<pmdk_vector_type>(options.num_ops, 1);
            });
            benchTwo = [&](int key1, int key2) {
                    pmem::obj::transaction::run(*vector_pool_ptr, [&] {
                            auto val1 = r->v_ptr->at(key1);
                            auto val2 = r->v_ptr->at(key2);
                            r->v_ptr->at(key1) = val2;
                            r->v_ptr->at(key2) = val1;
                    });
            };
        } else if (options.datastructure == "array") {
            auto array_pool = pmem::obj::pool<array_root>::create(options.backing_file.c_str(), "array",
                        poolSize, S_IWUSR|S_IRUSR);
            array_pool_ptr = &array_pool;
            a_r = array_pool_ptr->root();
            pmem::obj::transaction::run(*array_pool_ptr, [&] {
                a_r->v_ptr = pmem::obj::make_persistent<pmdk_array_type>();
            });
            a_r->v_ptr->fill(0);
            benchOne = [&](bool write, int key) {
                if (write) { 
                    pmem::obj::transaction::run(*array_pool_ptr, [&] {
                            a_r->v_ptr->at(key) = key;
                    });
                } else {
                    pmem::obj::transaction::run(*array_pool_ptr, [&] {
                        volatile auto val = a_r->v_ptr->const_at(key);
                    });
                }
            };
        } else if (options.datastructure == "queue") {
            if ((pop = pmemobj_create(options.backing_file.c_str(), POBJ_LAYOUT_NAME(queue),
                            poolSize, 0666)) == NULL) {
                cerr << "failed to create pool:" << pmemobj_errormsg() << endl;
                return;
            }
            TOID(struct fifo_root) root = POBJ_ROOT(pop, struct fifo_root);
            tqhead = &D_RW(root)->head;
	    // Need to handle cases when too many pops happen before pushes.
	    for (int i = 0; i < options.num_elems; i++) {
#ifndef PMDK_NO_TX
		TX_BEGIN(pop) {
		    qnode = TX_NEW(struct tqnode);
		    D_RW(qnode)->data = i;
		    POBJ_TAILQ_INSERT_HEAD(tqhead, qnode, tnd);
		} TX_ONABORT {
		    cerr << "transaction aborted:" << pmemobj_errormsg() << endl;
		    abort();
		} TX_END
#else
		POBJ_ZNEW(pop, &qnode, struct tqnode);
		D_RW(qnode)->data = i;
		POBJ_TAILQ_INSERT_HEAD(tqhead, qnode, tnd);
#endif
	    }
	    benchOne = [&](bool write, int key) {
#ifndef PMDK_NO_TX
		if (write) {
		    TX_BEGIN(pop) {
			qnode = TX_NEW(struct tqnode);
			D_RW(qnode)->data = key;
			POBJ_TAILQ_INSERT_HEAD(tqhead, qnode, tnd);
		    } TX_ONABORT {
			cerr << "transaction aborted:" << pmemobj_errormsg() << endl;
			abort();
		    } TX_END
		} else {
		    assert(!POBJ_TAILQ_EMPTY(tqhead));
		    qnode = POBJ_TAILQ_LAST(tqhead);
		    TX_BEGIN(pop) {
			//POBJ_TAILQ_REMOVE_FREE(tqhead, qnode, tnd);
			POBJ_TAILQ_REMOVE(tqhead, qnode, tnd);
		    } TX_ONABORT {
			cerr << "transaction aborted:" << pmemobj_errormsg() << endl;
			abort();
		    } TX_END
		}
#else
		if (write) {
		    POBJ_ZNEW(pop, &qnode, struct tqnode);
		    D_RW(qnode)->data = key;
		    POBJ_TAILQ_INSERT_HEAD(tqhead, qnode, tnd);
		} else {
		    qnode = POBJ_TAILQ_LAST(tqhead);
		    POBJ_TAILQ_REMOVE(tqhead, qnode, tnd);
		} 
#endif
	    };
	} else if (options.datastructure == "queue2") {
	    if ((pop = pmemobj_create(options.backing_file.c_str(), POBJ_LAYOUT_NAME(queue),
			    poolSize, 0666)) == NULL) {
		cerr << "failed to create pool:" << pmemobj_errormsg() << endl;
		return;
	    }
	    TOID(struct fifo_root) root = POBJ_ROOT(pop, struct fifo_root);
	    tqhead = &D_RW(root)->head;
	    // +1 because we pop one element at the beginning
	    for (int i = 0; i < options.num_elems+1; i++) {
#ifndef PMDK_NO_TX
		TX_BEGIN(pop) {
		    qnode = TX_NEW(struct tqnode);
		    D_RW(qnode)->data = i;
		    POBJ_TAILQ_INSERT_HEAD(tqhead, qnode, tnd);
		} TX_ONABORT {
		    cerr << "transaction aborted:" << pmemobj_errormsg() << endl;
		    abort();
		} TX_END
#else
		POBJ_ZNEW(pop, &qnode, struct tqnode);
		D_RW(qnode)->data = i;
		POBJ_TAILQ_INSERT_HEAD(tqhead, qnode, tnd);
#endif
	    }
	    qnode = POBJ_TAILQ_LAST(tqhead);
	    TX_BEGIN(pop) {
		//POBJ_TAILQ_REMOVE_FREE(tqhead, qnode, tnd);
		POBJ_TAILQ_REMOVE(tqhead, qnode, tnd);
	    } TX_ONABORT {
		cerr << "transaction aborted:" << pmemobj_errormsg() << endl;
		abort();
	    } TX_END
	    benchOne = [&](bool write, int key) {
#ifndef PMDK_NO_TX
		if (write) {
		    TX_BEGIN(pop) {
			qnode = TX_NEW(struct tqnode);
			D_RW(qnode)->data = key;
			POBJ_TAILQ_INSERT_HEAD(tqhead, qnode, tnd);
		    } TX_ONABORT {
			cerr << "transaction aborted:" << pmemobj_errormsg() << endl;
			abort();
		    } TX_END
		} else {
		    assert(!POBJ_TAILQ_EMPTY(tqhead));
		    qnode = POBJ_TAILQ_LAST(tqhead);
		    TX_BEGIN(pop) {
                        //POBJ_TAILQ_REMOVE_FREE(tqhead, qnode, tnd);
                        POBJ_TAILQ_REMOVE(tqhead, qnode, tnd);
                    } TX_ONABORT {
                        cerr << "transaction aborted:" << pmemobj_errormsg() << endl;
                        abort();
                    } TX_END
                }
#else
               if (write) {
                    POBJ_ZNEW(pop, &qnode, struct tqnode);
                    D_RW(qnode)->data = key;
                    POBJ_TAILQ_INSERT_HEAD(tqhead, qnode, tnd);
               } else {
                   qnode = POBJ_TAILQ_LAST(tqhead);
                   POBJ_TAILQ_REMOVE(tqhead, qnode, tnd);
               } 
#endif
            };
        } else if (options.datastructure == "stack") {
            if ((pop = pmemobj_create(options.backing_file.c_str(), POBJ_LAYOUT_NAME(list),
                            poolSize, 0666)) == NULL) {
                cerr << "failed to create pool:" << pmemobj_errormsg() << endl;
                return;
            }
            TOID(struct list_root) root = POBJ_ROOT(pop, struct list_root);
            tlhead = &D_RW(root)->head;
	    for (int i = 0; i < options.num_elems; i++) {
#ifndef PMDK_NO_TX
		TX_BEGIN(pop) {
		    lnode = TX_NEW(struct tlnode);
		    D_RW(lnode)->data = i;
		    POBJ_SLIST_INSERT_HEAD(tlhead, lnode, tnd);
		} TX_ONABORT {
		    cerr << "transaction aborted:" << pmemobj_errormsg() << endl;
		    abort();
		} TX_END
#else
		POBJ_ZNEW(pop, &lnode, struct tlnode);
		D_RW(lnode)->data = i;
		POBJ_SLIST_INSERT_HEAD(tlhead, lnode, tnd);
#endif
	    }
	    benchOne = [&](bool write, int key) {
#ifndef PMDK_NO_TX
		if (write) { // push front
		    TX_BEGIN(pop) {
			lnode = TX_NEW(struct tlnode);
			D_RW(lnode)->data = key;
			POBJ_SLIST_INSERT_HEAD(tlhead, lnode, tnd);
		    } TX_ONABORT {
			cerr << "transaction aborted:" << pmemobj_errormsg() << endl;
			abort();
		    } TX_END
		} else { // pop front 
		    assert(!POBJ_SLIST_EMPTY(tlhead));
		    lnode = POBJ_SLIST_FIRST(tlhead);
                    TX_BEGIN(pop) {
                        // POBJ_SLIST_REMOVE_FREE(tlhead, lnode, tnd);
                        POBJ_SLIST_REMOVE_HEAD(tlhead, tnd);
                    } TX_ONABORT {
                        cerr << "transaction aborted:" << pmemobj_errormsg() << endl;
                        abort();
                    } TX_END
                }
#else
                if (write) { // push front
                    POBJ_ZNEW(pop, &lnode, struct tlnode);
                    D_RW(lnode)->data = key;
                    POBJ_SLIST_INSERT_HEAD(tlhead, lnode, tnd);
                } else {
                    assert(!POBJ_SLIST_EMPTY(tlhead));
                    lnode = POBJ_SLIST_FIRST(tlhead);
                    POBJ_SLIST_REMOVE_HEAD(tlhead, tnd);
                }
#endif
            };

        } else if (options.datastructure == "array-swaps") {
            exp_pmdk_array = new pmdk_array_type();
            for (int i = 0; i < arraySize; i++) {
                (*exp_pmdk_array)[i] = i;
            }
            benchTwo = [&](int key1, int key2) {
                auto val1 = (*exp_pmdk_array)[key1];
                auto val2 = (*exp_pmdk_array)[key2];
                (*exp_pmdk_array)[key1] = val2; 
                (*exp_pmdk_array)[key2] = val1;
            };
        } else {
            cerr << "Unsupported datastructure:" << options.datastructure
                << endl;
            return;
        }
    } else if (options.impl == "dummy") {
        if (options.datastructure == "array") {
            tester = new concurrent_array();
            benchOne = [&](bool write, int key) {
                if (write) {
                    tester->set(key, 1000);
                } else {
                    volatile auto val = tester->get(key);
                }
            };
        } else if (options.datastructure == "locks") {
            std::shared_mutex mutex_;
            benchOne = [&](bool write, int key) {
                if (write) {
                    std::unique_lock<std::shared_mutex> lock(mutex_);
                } else {
                    std::shared_lock<std::shared_mutex> lock(mutex_);
                }
            };
        } else if (options.datastructure == "alloc") {
            nvm_initialize(options.backing_file.c_str(), 0);
            benchOne = [&](bool write, int key) {
                volatile void *mem = nvm_reserve(65);
                _mm_sfence();
            };
        } 
    } else {
        cerr << "Error! Unsupported implementation:" << options.impl << endl;
        return;
    }

    if (!options.enter_roi) {
        cout << "Exiting before ROI" << endl;
        return;
    }
    
#ifdef MEM_USAGE 
    if (options.impl == "immer") { 
        nvm_report_mem();
    } else if (options.impl == "pmdk") {
	report_mem_usage();
    }
#endif
    cout << "About to hit ROI" << endl;
    BeginValgrindInstrumentation();
    start = std::chrono::high_resolution_clock::now();
    for (int i=0; i < options.threads; i++) {
        workers[i] = std::thread(ThreadWork, i, options.ops_per_thread, benchOne, benchTwo);
    }
    for (int i=0; i < options.threads; i++) {
        workers[i].join();
    }
    stop = std::chrono::high_resolution_clock::now();

    // TODO: Re-enable checker when implemented correctly.
    //checker();

    auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>
        (stop-start).count();
    cout << "Latency: " << latency << " ns\n";
    cout << "Throughput: " << (options.num_ops*1E9/latency) << " op/s\n";

#ifdef MEM_USAGE 
    if (options.impl == "immer") { 
        nvm_report_mem();
    } else if (options.impl == "pmdk") {
	report_mem_usage();
    }
#endif
}

bool GetInputArgs(int argc, char **argv, ConfigOptions& options) {
    int opt;
    while ((opt = getopt(argc, argv, "d:e:o:n:i:f:t:w:p:s:r")) != -1) {
        switch (opt) {
            case 'i':
                options.impl = optarg;
                break;
            case 'd':
                options.datastructure = optarg;
                break;
            case 'e':
                options.num_elems = atol(optarg);
                break;
            case 'n':
                options.num_ops = atol(optarg);
                break;
            case 'f':
                options.backing_file = optarg;
                break;
            case 't':
                options.threads = atoi(optarg);
                break;
            case 'w':
                options.percent_writes = atoi(optarg);
                break;
            case 's':
                options.seed = atoi(optarg);
                break;
            case 'p':
                options.pattern = optarg;
                break;
            case 'r':
                options.enter_roi = false;
                break;
            default:
                PrintUsage();
                return false;
        }
    }
    if (options.datastructure == "vector-swaps")
        options.num_ops = 2*options.num_ops;

    options.ops_per_thread = options.num_ops / options.threads;
    if (options.ops_per_thread * options.threads != options.num_ops) {
        cerr << "Number of operations not divisble by number of threads"
            << endl;
        return false;
    }
    return true;
}

void PrintInputArgs(const ConfigOptions& options) {
    cout << "Configuration of current run:" << endl;
    cout << "Backing file:" << options.backing_file << endl;
    cout << "Implementation:" << options.impl << endl;
    cout << "Datastructure:" << options.datastructure << endl;
    cout << "Key/Index distribution:" << options.pattern << endl;
    cout << "Percent writes:" << options.percent_writes << "%" << endl;
    cout << "Number of elements:" << options.num_elems << endl;
    cout << "Number of operations:" << options.num_ops << endl;
    cout << "Number of threads:" << options.threads << endl;
    cout << "Enter ROI:" << options.enter_roi << endl;
}

int main (int argc, char **argv)
{
    ConfigOptions options;
    if (!GetInputArgs(argc, argv, options)) {
        cerr << "Exiting!" << endl;
        return 1;
    }
    PrintInputArgs(options);

    if (!options.Validate()) {
        PrintUsage();
        cerr << "Exiting!" << endl;
        return 1;
    }

    // We do not deal with backing file issues
    // as nvm_malloc uses a directory and pmdk uses
    // a file. Leads to much hassle.
    /*
       if (!SetUpBackingFile(options.backing_file.c_str())) {
       cerr << "Failed to delete backing file" << endl;
       return 1;
       }
    */

    RunBenchmark(options);

    return 0;
}

