CXX=g++
RM=rm -f

NVM_MALLOC_DIR=../nvm_malloc/
PMDK_DIR=../pmdk-1.5/src/
PMDK_CPP_DIR=../libpmemobj-cpp/include/
IMMER_DIR=../Immutable-Datastructure-c++/
VALGRIND_DIR=../pmem-valgrind/

CPPFLAGS=-O3 -ggdb -mclflushopt -std=c++17  -fopt-info -DNDEBUG -march=native # -mno-avx -mno-avx2 -fno-builtin-memmove
#Valgrind fails on some instruction when march=native
DEBUG_CPPFLAGS=-O2 -ggdb -mclflushopt -std=c++17 #-DIMMER_DEBUG -DNDEBUG 

INCLUDE_FLAGS=-I$(IMMER_DIR) -I$(NVM_MALLOC_DIR)/src/ -I$(PMDK_DIR)/include -I$(PMDK_DIR) -I$(PMDK_DIR)/common
INCLUDE_FLAGS+=-I$(PMDK_DIR)/examples/libpmemobj/hashmap/ -I$(PMDK_CPP_DIR) -I$(VALGRIND_DIR)/lackey/ -I$(VALGRIND_DIR)/include/

LDFLAGS=-L$(PMDK_DIR)/tx-nondebug -L$(NVM_MALLOC_DIR)
LDFLAGS+=-L$(PMDK_DIR)/examples/libpmemobj/hashmap/
LDFLAGS+=-lhashmap_tx -lset_tx -lnvmmalloc -lpmemobj -lpmem -pthread
MEM_LDFLAGS=-L$(PMDK_DIR)/tx-nondebug -L$(NVM_MALLOC_DIR)
MEM_LDFLAGS+=-L$(PMDK_DIR)/examples/libpmemobj/hashmap/
MEM_LDFLAGS+=-lhashmap_tx -lset_tx -lnvmmalloc -lpmemobj -lpmem -pthread
NOFLUSH_LDFLAGS=-L$(PMDK_DIR)/tx-nondebug -L$(NVM_MALLOC_DIR)
NOFLUSH_LDFLAGS+=-L$(PMDK_DIR)/examples/libpmemobj/hashmap/
NOFLUSH_LDFLAGS+=-lhashmap_tx -lset_tx -lnvmmallocnoflush -lpmemobj -lpmem -pthread

OBJS=bench.o debug_bench.o val_bench.o bench_no_flush.o memory_usage.o

TGTS=bench debug_bench val_bench bench_no_flush memory_usage 

all: $(TGTS)

bench.o: bench.cpp
	    $(CXX) -o $@ -c $< $(CPPFLAGS) $(INCLUDE_FLAGS)

bench_no_flush.o: bench.cpp
	    $(CXX) -o $@ -c $< $(CPPFLAGS) $(INCLUDE_FLAGS) -DIMMER_DISABLE_FLUSHING

val_bench.o: bench.cpp
	    $(CXX) -o $@ -c $< $(DEBUG_CPPFLAGS) $(INCLUDE_FLAGS) -DVALGRIND

debug_bench.o: bench.cpp
	    $(CXX) -o $@ -c $< $(DEBUG_CPPFLAGS) $(INCLUDE_FLAGS)

memory_usage.o: memory_usage.cpp
	    $(CXX) -o $@ -c $< $(CPPFLAGS) -DMEM_USAGE $(INCLUDE_FLAGS)

memory_usage: memory_usage.o
	    $(CXX) -o $@ $< $(MEM_LDFLAGS)

bench_no_flush: bench_no_flush.o
	    $(CXX) -o $@ $< $(NOFLUSH_LDFLAGS)

%: %.o
	    $(CXX) -o $@ $< $(LDFLAGS)

clean:
	$(RM) $(OBJS) $(TGTS)
