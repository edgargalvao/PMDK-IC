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

#export LD_LIBRARY_PATH=/home/swapnilh/pmdk/src/nondebug/:/home/swapnilh/pmdk/src/examples/libpmemobj/hashmap/:/home/swapnilh/pmdk-stuff/nvm_malloc/
export PMEM_NO_MOVNT=1
{
rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i pmdk -d map -f $PMEM/bench -e $1 -n $1 -t 1 -w 100 -p rand -s120

rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i pmdk -d set -f $PMEM/bench -e $1 -n $1 -t 1 -w 100 -p rand -s120

rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i pmdk -d queue -f $PMEM/bench -e $1 -n $1 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i pmdk -d queue -f $PMEM/bench -e $1 -n $1 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i pmdk -d stack -f $PMEM/bench -e $1 -n $1 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i pmdk -d stack -f $PMEM/bench -e $1 -n $1 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i pmdk -d vector -f $PMEM/bench -e $1 -n $1 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i pmdk -d vector-swaps -f $PMEM/bench -e $1 -n $1 -t 1 -w 100 -p rand -s120
} > pmdk_ordering.log 2> pmdk_val.log

{
rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i immer -d map -f $PMEM/bench -e $1 -n $1 -t 1 -w 100 -p rand -s120

rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i immer -d set -f $PMEM/bench -e $1 -n $1 -t 1 -w 100 -p rand -s120

rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i immer -d queue -f $PMEM/bench -e $1 -n $1 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i immer -d queue -f $PMEM/bench -e $1 -n $1 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i immer -d stack -f $PMEM/bench -e $1 -n $1 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i immer -d stack -f $PMEM/bench -e $1 -n $1 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i immer -d vector -f $PMEM/bench -e $1 -n $1 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes ./val_bench -i immer -d vector-swaps -f $PMEM/bench -e $1 -n $1 -t 1 -w 100 -p rand -s120
} > immer_ordering.log 2> immer_val.log
