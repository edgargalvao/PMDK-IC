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

{
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d queue -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d stack -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
} > immer_flush.log 2> immer_flush.log

{
rm -rf $PMEM/bench_no_flush*; taskset $CPU_GROUP ./bench_no_flush -i immer -d map -f $PMEM/bench_no_flush -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench_no_flush*; taskset $CPU_GROUP ./bench_no_flush -i immer -d map -f $PMEM/bench_no_flush -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench_no_flush*; taskset $CPU_GROUP ./bench_no_flush -i immer -d set -f $PMEM/bench_no_flush -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench_no_flush*; taskset $CPU_GROUP ./bench_no_flush -i immer -d set -f $PMEM/bench_no_flush -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench_no_flush*; taskset $CPU_GROUP ./bench_no_flush -i immer -d queue -f $PMEM/bench_no_flush -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench_no_flush*; taskset $CPU_GROUP ./bench_no_flush -i immer -d queue -f $PMEM/bench_no_flush -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench_no_flush*; taskset $CPU_GROUP ./bench_no_flush -i immer -d stack -f $PMEM/bench_no_flush -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench_no_flush*; taskset $CPU_GROUP ./bench_no_flush -i immer -d stack -f $PMEM/bench_no_flush -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench_no_flush*; taskset $CPU_GROUP ./bench_no_flush -i immer -d vector -f $PMEM/bench_no_flush -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench_no_flush*; taskset $CPU_GROUP ./bench_no_flush -i immer -d vector -f $PMEM/bench_no_flush -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120
rm -rf $PMEM/bench_no_flush*; taskset $CPU_GROUP ./bench_no_flush -i immer -d vector-swaps -f $PMEM/bench_no_flush -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
} > immer_no_flush.log 2> immer_no_flush.log


export PMEM_IS_PMEM_FORCE=1
export PMEM_MMAP_HINT=0x10000000000
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
export PMEM_NO_FLUSH=0
export PMEM_NO_FENCE=0
export LD_LIBRARY_PATH=/home/swapnilh/pmdk/src/nondebug/:/home/swapnilh/pmdk/src/examples/libpmemobj/hashmap/:/home/swapnilh/pmdk-stuff/nvm_malloc/
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
} > pmdk14.log 2> pmdk14.log
