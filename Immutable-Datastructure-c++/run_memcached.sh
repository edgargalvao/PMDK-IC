#!/bin/bash
export LD_LIBRARY_PATH=/home/swapnilh/pmdk-stuff/nvm_malloc/
rm -rf /mnt/ext4-pmem18/memcached*
MEMCACHED_DIR=/home/swapnilh/mnemosyne-gcc/usermode/
IMMER_DIR=/home/swapnilh/Immutable-Datastructure-c++/
taskset 0x100 $IMMER_DIR/build_vacation_o3/memcached-1.2.4/memcached /mnt/ext4-pmem18/memcached -u root -p 11211 -l 127.0.0.1 &
sleep 20
export LD_LIBRARY_PATH=${MEMCACHED_DIR}/library/:$LD_LIBRARY_PATH
${MEMCACHED_DIR}/bench/memcached/memslap -s 127.0.0.1:11211 -c 4 -x 100000 -T 4 -X 512 -F ${MEMCACHED_DIR}/run.cnf -d 1
pkill memcached
