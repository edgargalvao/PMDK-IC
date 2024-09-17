CPU_GROUP=0x1000
PMEM=/mnt/ext4-pmem14/
MEMCACHED_DIR=/home/swapnilh/mnemosyne-gcc/usermode/
IMMER_DIR=/home/swapnilh/Immutable-Datastructure-c++/
PMDK_DIR=/home/swapnilh/pmdk-stuff/

# DISABLE THP
echo never > /sys/kernel/mm/transparent_hugepage/enabled
# DISABLE NUMA BALANCING
echo 0 > /proc/sys/kernel/numa_balancing
# DISABLE HW prefetchers?
bash control_hw_prefetch.sh d

make clean; make -j12

source source.sh
export LD_LIBRARY_PATH=${MEMCACHED_DIR}/library/:$LD_LIBRARY_PATH
# for map, num of elements is useless

{
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d queue2 -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120
} > immer_flush.queue 2>> error.queue

{
rm -rf $PMEM/bench_no_flush*; taskset $CPU_GROUP ./bench_no_flush -i immer -d queue2 -f $PMEM/bench_no_flush -e 10000 -n 10000 -t 1 -w 0 -p rand -s120
} > immer_no_flush.queue 2>> error.queue

{
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue2 -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120
} > pmdk_flush.queue 2>> error.queue 

{
export PMEM_NO_FLUSH=1
export PMEM_NO_FENCE=1
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue2 -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120
} > pmdk_no_flush.queue 2>> error.queue

{
export PMEM_NO_FLUSH=1
export PMEM_NO_FENCE=1
export LD_LIBRARY_PATH=/home/swapnilh/pmdk-stuff/pmdk//src/notx-nondebug/:/home/swapnilh/pmdk-stuff/nvm_malloc/:$LD_LIBRARY_PATH
m -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue2 -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120
} > pmdk_no_queue.queue 2>> error.queue

{
export PMEM_NO_FLUSH=0
export PMEM_NO_FENCE=0
export LD_LIBRARY_PATH=/home/swapnilh/pmdk/src/nondebug/:/home/swapnilh/pmdk/src/examples/libpmemobj/hashmap/:/home/swapnilh/pmdk-stuff/nvm_malloc/:$LD_LIBRARY_PATH
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue2 -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

} > pmdk14.queue 2>> error.queue
