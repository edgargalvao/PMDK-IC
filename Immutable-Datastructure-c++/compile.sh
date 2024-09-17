rm -rf build_vacation
mkdir build_vacation
cd build_vacation
cmake ../
make vacation vacation_no_flush memcached memcached_no_flush -j4
