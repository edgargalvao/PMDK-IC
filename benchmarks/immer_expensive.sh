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
rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes --trace-mem=yes --trace-flush=yes ./val_bench -i immer -d map -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 100 -p rand -s120 > $PMEM/map.log 2> $PMEM/map.log

rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes --trace-mem=yes --trace-flush=yes ./val_bench -i immer -d set -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 100 -p rand -s120 > $PMEM/set.log 2> $PMEM/set.log
rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes --trace-mem=yes --trace-flush=yes ./val_bench -i immer -d queue -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 100 -p rand -s120 > $PMEM/queue.log 2> $PMEM/queue.log
rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes --trace-mem=yes --trace-flush=yes ./val_bench -i immer -d queue -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120> $PMEM/queue2.log 2> $PMEM/queue2.log

rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes --trace-mem=yes --trace-flush=yes ./val_bench -i immer -d stack -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 100 -p rand -s120> $PMEM/stack1.log 2> $PMEM/stack1.log
rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes --trace-mem=yes --trace-flush=yes ./val_bench -i immer -d stack -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120> $PMEM/stack2.log 2> $PMEM/stack2.log

rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes --trace-mem=yes --trace-flush=yes ./val_bench -i immer -d vector -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 100 -p rand -s120> $PMEM/vec1.log 2> $PMEM/vec1.log
rm -rf $PMEM/bench*; valgrind --tool=lackey --flush-counts=yes --trace-mem=yes --trace-flush=yes ./val_bench -i immer -d vector-swaps -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 100 -p rand -s120 > $PMEM/vec2.log 2> $PMEM/vec2.log

python checker.py $PMEM/map.log
python checker.py $PMEM/set.log
python checker.py $PMEM/queue1.log
python checker.py $PMEM/queue2.log
python checker.py $PMEM/stack1.log
python checker.py $PMEM/stack2.log
python checker.py $PMEM/vec1.log
python checker.py $PMEM/vec2.log
