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

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d queue -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d stack -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i immer -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
} > immer_now.log 2> immer_now.log
