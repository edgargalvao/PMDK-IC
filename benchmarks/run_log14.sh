CPU_GROUP=0x1000
PMEM=/mnt/ext4-pmem14/
MEMCACHED_DIR=/home/swapnilh/mnemosyne-gcc/usermode/
IMMER_DIR=/home/swapnilh/Immutable-Datastructure-c++/
PMDK_APP_DIR=/home/swapnilh/pmdk-stuff/
PMDK14_DIR=/home/swapnilh/pmdk/src

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
export PMEM_NO_FLUSH=1
export PMEM_NO_FENCE=1
export LD_LIBRARY_PATH=${PMDK14_DIR}/notx-nondebug2/::$LD_LIBRARY_PATH
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d map -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d set -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d queue -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d stack -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120

rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 50 -p rand -s120
rm -rf $PMEM/bench*; taskset $CPU_GROUP ./bench -i pmdk -d vector-swaps -f $PMEM/bench -e 1000000 -n 1000000 -t 1 -w 100 -p rand -s120

rm -rf $PMEM/vacation; taskset $CPU_GROUP  $PMDK_APP_DIR/vacation/build/vacation $PMEM/vacation  -r100000 -t200000 -n1 -q80 -u55;
sleep 1
rm -rf $PMEM/memcached*; taskset $CPU_GROUP $PMDK_APP_DIR/memcached-1.2.4/build/memcached $PMEM/memcached -u root -p 11211 -l 127.0.0.1 &
sleep 10
${MEMCACHED_DIR}/bench/memcached/memslap -s 127.0.0.1:11211 -c 4 -x 100000 -T 4 -X 512 -F ${MEMCACHED_DIR}/run.cnf -d 1
pkill memcached

} > pmdk14_no_log.log 2>> error.log
