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
#include <immer/concurrent_map.hpp>
#include <immer/concurrent_vector.hpp>
#include <immer/set.hpp>
#include <immer/array.hpp>
#include <immer/vector.hpp>
#include <include/libpmemobj.h>
#include <include/libpmemobj/tx.h>
#include <libpmemobj++/experimental/vector.hpp>

#include "concurrent_array.hpp"

extern "C"
{
#include <examples/libpmemobj/hashmap/hashmap_tx.h>
#include <examples/libpmemobj/map/map.h>
#include <lackey.h>
}

using namespace std;

//common stuff
constexpr auto maxThreads = 8;
constexpr auto poolSize = 16*1024*1024*1024UL;
using bench_type = std::function<void(bool, int)>;
using check_type = std::function<void(void)>;

// PMDK stuff
#define OBJ_TYPE_NUM 1

POBJ_LAYOUT_BEGIN(layout_map);
POBJ_LAYOUT_ROOT(layout_map, struct root);
POBJ_LAYOUT_END(layout_map);

struct root {
	TOID(struct map) map;
};

static TOID(struct root) root;


POBJ_LAYOUT_BEGIN(layout_array);
POBJ_LAYOUT_TOID(layout_array, int);
POBJ_LAYOUT_END(layout_array);

PMEMobjpool *pop;

// IMMER stuff
using keyType = uint64_t;
constexpr auto keySize = sizeof(keyType);
constexpr auto valueSize = 32;
using valueType = std::array<char, valueSize>;

using map_type = immer::map<keyType, valueType>;
using concurrent_map_type = immer::concurrent_map<keyType, valueType>;
using set_type = immer::set<keyType>;
using vector_type = immer::vector<uint64_t>;
using concurrent_vector_type = immer::concurrent_vector<uint64_t>;
using array_type = immer::array<uint64_t>;
using pmdk_vector_type = pmem::obj::experimental::vector<uint64_t>;

// Global Definitions
//
// Datastructures defined here to ensure they stay in scope for the benchmark lambda
map_type* immer_map;
vector_type* immer_vector;
concurrent_vector_type* concurrent_immer_vector;
concurrent_map_type* concurrent_immer_map;
array_type* immer_array;

int *keys;
bool *writes;
valueType value;
PMEMoid value_ptr; 
PMEMoid pmdk_array;
int *array_direct;
TOID(struct hashmap_tx) *map; 
concurrent_array *tester;

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

    bool Validate () {
        return (!backing_file.empty() && !datastructure.empty()
             && !impl.empty() && (threads >0) && (num_ops > 0)
             && (num_elems > 0) && (num_ops <= num_elems));

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
        " -n num_ops -f backing-file -t threads\n" << endl;
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
        bench_type bench) {

    uint64_t thread_offset = thread_id*ops_per_thread;
    for (uint64_t i = 0; i < ops_per_thread; i++) {
        auto offset = thread_offset+i;
        auto key = keys[offset];
        auto write = writes[offset];
        bench(write, key);
    }
}

void BeginValgrindInstrumentation() {
    // Stop and Start flushes all counters
    LACKEY_STOP_INSTRUMENTATION;
    LACKEY_START_INSTRUMENTATION;
}

// TODO: Add checker for all benchmarks
void RunBenchmark (const ConfigOptions& options) {

    std::thread workers[maxThreads];
    std::chrono::time_point<std::chrono::high_resolution_clock> start, stop;
    bench_type bench;
    std::function<void(void)> checker;
    keys = new int [options.num_ops];
    writes = new bool [options.num_ops];

    PopulateKeysAndOps(options, keys, writes);

    if (options.impl == "immer") {
        nvm_initialize(options.backing_file.c_str(), 0);
        if (options.datastructure == "map") {
            immer_map = new map_type(); 
            GenerateZeroValue(value);
            // Prepopulate the datastructure
            for (int i = 0; i < options.num_elems; i++) {
                immer_map = immer_map->set_ptr(i, value);
            }
            if (options.threads == 1) {
                GenerateValue(value);
                bench = [&](bool write, int key) {
                    if (write) {
                        auto* old_map = immer_map;
                        immer_map = immer_map->set_ptr(key, value);
                        _mm_sfence();
                        delete old_map;
                    } else {
                        volatile auto* val = immer_map->find(key);
                    }
                };
            } else {
                GenerateValue(value);
                concurrent_immer_map = new concurrent_map_type(&immer_map);
                bench = [&](bool write, int key) {
                    // Concurrent_immer_map is neither immutable nor
                    // persistent, but just a wrapper around an immutable
                    // and persistent map.
                    if (write) {
                        concurrent_immer_map->set(key, value);
                    } else {
                        volatile auto* val = immer_map->find(key);
                    }
                };
            }
            checker = [&]() {
                assert(immer_map->size() == options.num_ops);
            };
        } else if (options.datastructure == "vector") {
            immer_vector = new vector_type(options.num_elems, 0);
            if (options.threads == 1) {
                bench = [&](bool write, int key) {
                    if (write) { 
                        auto* old_vector = immer_vector;
                        immer_vector = immer_vector->push_back_ptr(key);
                        _mm_sfence();
                        delete old_vector;
                    } else {
                        cerr << "Reads not supported for vector yet";
                        assert(0);
                    }
                };
            } else {
                concurrent_immer_vector =
                    new concurrent_vector_type(&immer_vector);
                bench = [&](bool write, int key) {
                    if (write) {
                        concurrent_immer_vector->push_back(key);
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
        } else if (options.datastructure == "array") {
            assert(options.threads == 1);
            immer_array = new array_type(options.num_elems, 0); 
            bench = [&](bool write, int key) {
                if(write) {
                    auto* old_array = immer_array;
                    immer_array = immer_array->set_ptr(key, key);
                    _mm_sfence();
                    delete old_array;
                } else {
                    volatile auto val = immer_array->at(key);
                }
            };
        } else if (options.datastructure == "array-swaps") {
            assert(options.threads == 1);
            immer_array = new array_type(options.num_elems, 0); 
            bench = [&](bool write, int key) {
                if(write) {
                    auto* old_array = immer_array;
                    immer_array = immer_array->set_ptr(key, key);
                    _mm_sfence();
                    delete old_array;
                } else {
                    volatile auto val = immer_array->at(key);
                }
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
            }            root = POBJ_ROOT(pop, struct root);
            map = (TOID(struct hashmap_tx) *) &D_RW(root)->map;
            if (hm_tx_create(pop, map, NULL)) {
                perror("map_new");
                return;
            } 
            // Prepopulate the datastructure
            pmemobj_zalloc(pop, &value_ptr, sizeof(uint64_t), OBJ_TYPE_NUM);
            for (int i = 0; i < options.num_elems; i++) {
                hm_tx_insert(pop, *map, i, value_ptr); 
            }
            bench = [&](bool write, int key) {
                if (write) {
                    // We allocate explicitly here as insert
                    // needs pointer to allocated value.
                    // Zalloc zeroes out location, which acts as dummy of 
                    // actually updating location with real value.
                    // We want to actually use alloc and then write in real
                    // value, but hard to do as it takes parameter contructor.
                    pmemobj_zalloc(pop, &value_ptr, sizeof(uint64_t), OBJ_TYPE_NUM);
                    hm_tx_insert(pop, *map, key, value_ptr); 
                } else {
                    volatile auto val = hm_tx_get(pop, *map, key);
                }
            };
        } else if (options.datastructure == "vector") {
           /*
            * pmdk_vector_type pmdk_vector = new pmdk_vector_type(options.num_elems, 0);
           bench = [&](bool write, int key) {
               if (write) { 
                   pmdk_vector->push_back(key);
               } else {
                   cerr << "Reads not supported for vector yet";
                   assert(0);
               }
           };
           */
            assert(0);
        } else if (options.datastructure == "array") {
            if ((pop = pmemobj_create(options.backing_file.c_str(),
                            POBJ_LAYOUT_NAME(layout_array),
                            poolSize, 0666)) == NULL) {
                cerr << "failed to create pool:" << pmemobj_errormsg() << endl;
                return;
            }
			/*
			 * To allocate persistent array of simple type is enough to allocate
			 * pointer with size equal to number of elements multiplied by size of
			 * user-defined structure.
			 */
			int ret = pmemobj_zalloc(pop, &pmdk_array,
                    sizeof(int)*options.num_elems, OBJ_TYPE_NUM);
			if (ret) {
				cerr << "Error allocating memory for array: " << pmemobj_errormsg() << endl;
				return;
			}
            int *array_direct = (int*) pmemobj_direct(pmdk_array);
            bench = [&](bool write, int key) {
                if (write) {
                    TX_BEGIN(pop) {
                        pmemobj_tx_add_range_direct(array_direct+key, sizeof(int));
                        array_direct[key] = key;
                    } TX_ONABORT {
                        cerr << "Transaction aborted:" << pmemobj_errormsg();
                        // ret = -1;
                    } TX_END
                } else {
                    volatile auto val = array_direct[key];
                }
            };
        } else {
            cerr << "Unsupported datastructure:" << options.datastructure
                << endl;
            return;
        }
    } else if (options.impl == "dummy") {
        if (options.datastructure == "array") {
            tester = new concurrent_array();
            bench = [&](bool write, int key) {
                if (write) {
                    tester->set(key, 1000);
                } else {
                    volatile auto val = tester->get(key);
                }
            };
        } else if (options.datastructure == "locks") {
            std::shared_mutex mutex_;
            bench = [&](bool write, int key) {
                if (write) {
                    std::unique_lock<std::shared_mutex> lock(mutex_);
                } else {
                    std::shared_lock<std::shared_mutex> lock(mutex_);
                }
            };
        } else if (options.datastructure == "alloc") {
            nvm_initialize(options.backing_file.c_str(), 0);
            bench = [&](bool write, int key) {
                volatile void *mem = nvm_reserve(65);
                _mm_sfence();
            };
        } 
    } else {
        cerr << "Error! Unsupported implementation:" << options.impl << endl;
        return;
    }

    BeginValgrindInstrumentation();
    start = std::chrono::high_resolution_clock::now();
    for (int i=0; i < options.threads; i++) {
        workers[i] = std::thread(ThreadWork, i, options.ops_per_thread, bench);
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

    if (options.impl == "immer") {
        nvm_report_mem();
    }
}

bool GetInputArgs(int argc, char **argv, ConfigOptions& options) {
    int opt;
    while ((opt = getopt(argc, argv, "d:e:o:n:i:f:t:w:p:s:")) != -1) {
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
            default:
                PrintUsage();
                return false;
        }
    }

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
    cout << "Datastructure:" << options.datastructure << endl;
    cout << "Key/Index distribution:" << options.pattern << endl;
    cout << "Percent writes:" << options.percent_writes << "%" << endl;
    cout << "Number of elements:" << options.num_elems << endl;
    cout << "Number of operations:" << options.num_ops << endl;
    cout << "Number of threads:" << options.threads << endl;
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

