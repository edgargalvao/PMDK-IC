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
export LD_LIBRARY_PATH=/home/swapnilh/pmdk-stuff/pmdk/src/nondebug/:/home/swapnilh/pmdk-stuff/nvm_malloc/

make clean; make -j12

source source.sh
export LD_LIBRARY_PATH=${MEMCACHED_DIR}/library/:$LD_LIBRARY_PATH
# for map, num of elements is useless

{
export PMEM_NO_FLUSH=0
export PMEM_NO_FENCE=0
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue2 -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d stack -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120

rm -rf $PMEM/vacation; taskset $CPU_GROUP  $PMDK_DIR/vacation/build/vacation $PMEM/vacation  -r100000 -t200000 -n1 -q80 -u55;
sleep 1
rm -rf $PMEM/memcached*; taskset $CPU_GROUP $PMDK_DIR/memcached-1.2.4/build/memcached $PMEM/memcached -u root -p 11211 -l 127.0.0.1 &
sleep 10
${MEMCACHED_DIR}/bench/memcached/memslap -s 127.0.0.1:11211 -c 4 -x 100000 -T 4 -X 512 -F ${MEMCACHED_DIR}/run.cnf -d 1
pkill memcached

} > pmdk_flush_thesis.log 2>> error_thesis.log 

{
export PMEM_NO_FLUSH=1
export PMEM_NO_FENCE=1
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue2 -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d stack -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120

rm -rf $PMEM/vacation; taskset $CPU_GROUP  $PMDK_DIR/vacation/build/vacation $PMEM/vacation  -r100000 -t200000 -n1 -q80 -u55;
sleep 1
rm -rf $PMEM/memcached*; taskset $CPU_GROUP $PMDK_DIR/memcached-1.2.4/build/memcached $PMEM/memcached -u root -p 11211 -l 127.0.0.1 &
sleep 10
${MEMCACHED_DIR}/bench/memcached/memslap -s 127.0.0.1:11211 -c 4 -x 100000 -T 4 -X 512 -F ${MEMCACHED_DIR}/run.cnf -d 1
pkill memcached

} > pmdk_no_flush_thesis.log 2>> error_thesis.log

{
export PMEM_NO_FLUSH=0
export PMEM_NO_FENCE=1
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue2 -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d stack -f $PMEM/bench -e 10000 -n 10000 -t 1 -w 0 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 0 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120

rm -rf $PMEM/vacation; taskset $CPU_GROUP  $PMDK_DIR/vacation/build/vacation $PMEM/vacation  -r100000 -t200000 -n1 -q80 -u55;
sleep 1
rm -rf $PMEM/memcached*; taskset $CPU_GROUP $PMDK_DIR/memcached-1.2.4/build/memcached $PMEM/memcached -u root -p 11211 -l 127.0.0.1 &
sleep 10
${MEMCACHED_DIR}/bench/memcached/memslap -s 127.0.0.1:11211 -c 4 -x 100000 -T 4 -X 512 -F ${MEMCACHED_DIR}/run.cnf -d 1
pkill memcached

} > pmdk_no_order_thesis.log 2>> error_thesis.log

