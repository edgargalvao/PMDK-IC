#!/bin/bash
export LD_LIBRARY_PATH=/home/swapnilh/pmdk-stuff/nvm_malloc/
MEMCACHED_DIR=/home/swapnilh/mnemosyne-gcc/usermode/
IMMER_DIR=/home/swapnilh/Immutable-Datastructure-c++/
PMDK_DIR=/home/swapnilh/pmdk-stuff/
PMEM=/mnt/ext4-pmem14/
CPU_GROUP=0x1000

export LD_LIBRARY_PATH=/home/swapnilh/pmdk-stuff/pmdk/src/tx-nondebug/:/home/swapnilh/pmdk-stuff/nvm_malloc/
export PMEM_IS_PMEM_FORCE=1
export PMEM_MMAP_HINT=0x10000000000
export PMEM_NO_MOVNT=1
export PMEM_NO_FLUSH=0
export PMEM_NO_FENCE=0

{
export PMEM_NO_FLUSH=0
export PMEM_NO_FENCE=0

rm -rf $PMEM/vacation; taskset $CPU_GROUP  $PMDK_DIR/vacation/build/vacation $PMEM/vacation  -r100000 -t200000 -n1 -q80 -u55;
rm -rf $PMEM/memcached*; taskset $CPU_GROUP $PMDK_DIR/memcached-1.2.4/build/memcached $PMEM/memcached -u root -p 11211 -l 127.0.0.1 &
sleep 10
export LD_LIBRARY_PATH=${MEMCACHED_DIR}/library/:$LD_LIBRARY_PATH
${MEMCACHED_DIR}/bench/memcached/memslap -s 127.0.0.1:11211 -c 4 -x 100000 -T 4 -X 512 -F ${MEMCACHED_DIR}/run.cnf -d 1
pkill memcached
} 
