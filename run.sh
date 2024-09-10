CPU_GROUP=0x10
PMEM=$1/
MOD_DIR=Immutable-Datastructure-c++/
VACATION_DIR=vacation-pmdk/
MEMCACHED_DIR=memcached-pmdk
MEMSLAP_DIR=memslap
BFS_DIR=graph-algo/
BENCH_DIR=benchmarks/
PREFIX="taskset $CPU_GROUP"
CLEANUP="rm -rf $PMEM*; echo '========================='"


if [ "$1" = "" ]
then
  echo "Usage: $0 <path-on-pmem-or-disk>"
  exit
fi

if [ -d "$1" ]; then
  echo "Directory $1 already exists. Script will delete everything in the provided directory so please provide a new directory name."
  exit
fi

# DISABLE THP
echo never > /sys/kernel/mm/transparent_hugepage/enabled
# DISABLE NUMA BALANCING
echo 0 > /proc/sys/kernel/numa_balancing
# DISABLE HW prefetchers?
bash control_hw_prefetch.sh d

#make clean; make -j12
echo "Creating new directory:" $PMEM
mkdir $PMEM

echo "Deleting old logs"
rm -f *.log

source source.sh

echo "Check error.log for any errors!"

echo "Running MOD workloads, results in mod_flush.log"
{
eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i immer -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i immer -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i immer -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i immer -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i immer -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120
eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i immer -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120

eval $CLEANUP; $PREFIX  $MOD_DIR/build_vacation/vacation/flush/vacation $PMEM/vacation  -r100000 -t200000 -n1 -q55 -u99
sleep 1
eval $CLEANUP; $PREFIX $MOD_DIR/build_vacation/memcached-1.2.4/flush/memcached $PMEM/memcached -u root -p 11211 -l 127.0.0.1 &
sleep 10
${MEMSLAP_DIR}/memslap -s 127.0.0.1:11211 -c 4 -x 100000 -T 4 -X 512 -F ${MEMCACHED_DIR}/run.cnf -d 1
pkill memcached

eval $CLEANUP; $PREFIX $BFS_DIR/mod_bfs $PMEM/bfs $BFS_DIR/flickr.mtx0 
} > mod_flush.log 2>> error.log

echo "Running MOD workloads w/o flushing, results in mod_no_flush.log"
{
eval $CLEANUP; $PREFIX $BENCH_DIR/bench_no_flush -i immer -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench_no_flush -i immer -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench_no_flush -i immer -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench_no_flush -i immer -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench_no_flush -i immer -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120
eval $CLEANUP; $PREFIX $BENCH_DIR/bench_no_flush -i immer -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
sleep 2
eval $CLEANUP; $PREFIX  $MOD_DIR/build_vacation/vacation/no_flush/vacation_no_flush $PMEM/vacation -r100000 -t200000 -n1 -q55 -u99
sleep 2
eval $CLEANUP; $PREFIX $MOD_DIR/build_vacation/memcached-1.2.4/no_flush/memcached_no_flush $PMEM/memcached -u root -p 11211 -l 127.0.0.1 &
sleep 10
${MEMSLAP_DIR}/memslap -s 127.0.0.1:11211 -c 4 -x 100000 -T 4 -X 512 -F ${MEMSLAP_DIR}/run.cnf -d 1
pkill memcached
sleep 4;
eval $CLEANUP; $PREFIX $BFS_DIR/mod_bfs_noflush $PMEM/bfs $BFS_DIR/flickr.mtx0 
} > mod_no_flush.log 2>> error.log

echo "Running PMDK workloads, results in pmdk.log"
{
eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120
eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120

eval $CLEANUP;  $PREFIX  $VACATION_DIR/build/vacation $PMEM/vacation  -r100000 -t200000 -n1 -q80 -u55;
sleep 1
eval $CLEANUP; mkdir $PMEM/memcached; $PREFIX $MEMCACHED_DIR/build/memcached $PMEM/memcached -u root -p 11211 -l 127.0.0.1 &
sleep 10
${MEMSLAP_DIR}/memslap -s 127.0.0.1:11211 -c 4 -x 100000 -T 4 -X 512 -F ${MEMSLAP_DIR}/run.cnf -d 1
pkill memcached

eval $CLEANUP; $PREFIX $BFS_DIR/pmdk_bfs $PMEM/bfs $BFS_DIR/flickr.mtx0 
} > pmdk.log 2>> error.log 

echo "Running PMDK workloads w/o flushes, results in pmdk_no_flush.log"
{
export PMEM_NO_FLUSH=1
export PMEM_NO_FENCE=1
eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120
eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120

eval $CLEANUP; $PREFIX  $VACATION_DIR/build/vacation $PMEM/vacation  -r100000 -t200000 -n1 -q80 -u55;
sleep 1
eval $CLEANUP; mkdir $PMEM/memcached; $PREFIX $MEMCACHED_DIR/build/memcached $PMEM/memcached -u root -p 11211 -l 127.0.0.1 &
sleep 10
${MEMSLAP_DIR}/memslap -s 127.0.0.1:11211 -c 4 -x 100000 -T 4 -X 512 -F ${MEMSLAP_DIR}/run.cnf -d 1
pkill memcached

eval $CLEANUP; $PREFIX $BFS_DIR/pmdk_bfs $PMEM/bfs $BFS_DIR/flickr.mtx0 
} > pmdk_no_flush.log 2>> error.log

echo "Running PMDK workloads w/o flushing or logging, results in pmdk_no_log.log"
{
export PMEM_NO_FLUSH=1
export PMEM_NO_FENCE=1
export LD_LIBRARY_PATH=pmdk/src/notx-nondebug/:$LD_LIBRARY_PATH
eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120
eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120
eval $CLEANUP; $PREFIX $BENCH_DIR/bench -i pmdk -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120

eval $CLEANUP; $PREFIX  $VACATION_DIR/build/vacation_notx $PMEM/vacation  -r100000 -t200000 -n1 -q80 -u55;
sleep 1
eval $CLEANUP; mkdir $PMEM/memcached; $PREFIX $MEMCACHED_DIR/build/memcached $PMEM/memcached -u root -p 11211 -l 127.0.0.1 &
sleep 10
${MEMSLAP_DIR}/memslap -s 127.0.0.1:11211 -c 4 -x 100000 -T 4 -X 512 -F ${MEMSLAP_DIR}/run.cnf -d 1
pkill memcached

eval $CLEANUP;$PREFIX $BFS_DIR/pmdk_bfs $PMEM/bfs $BFS_DIR/flickr.mtx0 
} > pmdk_no_log.log 2>> error.log
