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
# for map, num of elements is useless

{
rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120 -r
rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120 -r

rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120 -r
rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120 -r

rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120 -r
rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d queue -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120 -r

rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120 -r
rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d stack -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120 -r

rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120 -r
rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120 -r
rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120 -r

rm -rf $PMEM/vacation; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses   $IMMER_DIR/build_vacation_o3/vacation/flush/vacation $PMEM/vacation  -r100000 -t200000 -n1 -q55 -u99
sleep 1
rm -rf $PMEM/memcached*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  $IMMER_DIR/build_vacation_o3/memcached-1.2.4/flush/memcached $PMEM/memcached -u root -p 11211 -l 127.0.0.1 &
sleep 20
export LD_LIBRARY_PATH=${MEMCACHED_DIR}/library/:$LD_LIBRARY_PATH
${MEMCACHED_DIR}/bench/memcached/memslap -s 127.0.0.1:11211 -c 4 -x 100000 -T 4 -X 512 -F ${MEMCACHED_DIR}/run.cnf -d 1
pkill memcached

} > immer_refs_baseline_misses_config.log 2> immer_refs_baseline_misses.log


{
rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d queue -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d stack -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120
rm -rf $PMEM/bench*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  ./bench -i immer -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120

rm -rf $PMEM/vacation; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses   $IMMER_DIR/build_vacation_o3/vacation/flush/vacation $PMEM/vacation  -r100000 -t200000 -n1 -q55 -u99
sleep 1
rm -rf $PMEM/memcached*; perf stat -e L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,LLC-load-misses  $IMMER_DIR/build_vacation_o3/memcached-1.2.4/flush/memcached $PMEM/memcached -u root -p 11211 -l 127.0.0.1 &
sleep 20
export LD_LIBRARY_PATH=${MEMCACHED_DIR}/library/:$LD_LIBRARY_PATH
${MEMCACHED_DIR}/bench/memcached/memslap -s 127.0.0.1:11211 -c 4 -x 100000 -T 4 -X 512 -F ${MEMCACHED_DIR}/run.cnf -d 1
pkill memcached

} > immer_refs_misses_config.log 2> immer_refs_misses.log
