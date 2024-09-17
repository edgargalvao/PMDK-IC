CPU_GROUP=0x1000
PMEM=/mnt/ext4-pmem14/
# DISABLE THP
echo never > /sys/kernel/mm/transparent_hugepage/enabled
# DISABLE NUMA BALANCING
echo 0 > /proc/sys/kernel/numa_balancing
# DISABLE HW prefetchers?
bash control_hw_prefetch.sh d

make clean; make -j12

source source.sh
# for map, num of elements is useless

export PMEM_IS_PMEM_FORCE=1
export PMEM_MMAP_HINT=0x10000000000
export PMEM_NO_MOVNT=1
{
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d stack -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
} > pmdk_flush.log 2> pmdk_flush.log

{
export PMEM_NO_FLUSH=1
export PMEM_NO_FENCE=1
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d stack -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
} > pmdk_no_flush.log 2> pmdk_no_flush.log


{
export PMEM_NO_FLUSH=1
export PMEM_NO_FENCE=1
export LD_LIBRARY_PATH=/home/swapnilh/pmdk-stuff/pmdk//src/notx-nondebug/:/home/swapnilh/pmdk-stuff/nvm_malloc/
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d stack -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
} > pmdk_no_log.log 2> pmdk_no_log.log
