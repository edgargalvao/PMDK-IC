CPU_GROUP=0x1000
PMEM=/mnt/ext4-pmem14/
# DISABLE THP
echo never > /sys/kernel/mm/transparent_hugepage/enabled
# DISABLE NUMA BALANCING
echo 0 > /proc/sys/kernel/numa_balancing
# DISABLE HW prefetchers?
bash control_hw_prefetch.sh d

make clean; make -j12

ops=100000

# for map, num of elements is useless
{
# These defines are just to make it run faster.
export PMEM_NO_MOVNT=1
export PMEM_NO_FLUSH=1
export PMEM_NO_FENCE=1
export PMEM_MMAP_HINT=0x10000000000
export LD_LIBRARY_PATH=/home/swapnilh/pmdk-stuff/pmdk/src/nondebug/:/home/swapnilh/pmdk-stuff/nvm_malloc/
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./memory_usage -i pmdk -d map -f $PMEM/bench -e $ops -n $ops -t 1 -w 100 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./memory_usage -i pmdk -d set -f $PMEM/bench -e $ops -n $ops -t 1 -w 100 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./memory_usage -i pmdk -d queue -f $PMEM/bench -e $ops -n $ops -t 1 -w 50 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./memory_usage -i pmdk -d stack -f $PMEM/bench -e $ops -n $ops -t 1 -w 50 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./memory_usage -i pmdk -d vector -f $PMEM/bench -e $ops -n $ops -t 1 -w 100 -p rand -s120
} > pmdk_memory.log 2> pmdk_memory.log
