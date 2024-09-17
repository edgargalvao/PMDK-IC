//
// immer: immutable data structures for C++
// Copyright (C) 2016, 2017, 2018 Juan Pedro Bolivar Puente
//
// This software is distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://boost.org/LICENSE_1_0.txt
//

#include <immer/box.hpp>
#include <cassert>
#include <array>
#include <cstring>
#include <cstdint>
#include <nvm_malloc.h>
#include <iostream>

using box_type = immer::box<std::array<int, 64>>;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "Error! Correct Usage: ./simple_ctr <1/0>, 1 for recovery\n";
        return 0;
    }
    int option = atoi(argv[1]);
    if (option == 0) {
        nvm_initialize("/dev/shm/box/", 0);
        box_type *mem = (box_type *) nvm_reserve_id("box", sizeof(box_type));
        std::cout << "Box located at:" << (void*) mem << std::endl;
        box_type* v1 = new (mem) box_type{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63};
        nvm_persist(mem, sizeof(box_type));
        nvm_activate_id("box");
        std::cout << "Original Value:" << v1->get()[62] << std::endl;
    } else {
        // recovered
        nvm_initialize("/dev/shm/box/", 1); 
        box_type* box_recovered = (box_type*) nvm_get_id("box");
        std::cout << "Box recovered at:" << (void*) box_recovered << std::endl;
        const auto recovered_value = box_recovered->get();
        std::cout << "Recovered value:" << recovered_value[62] << std::endl;        
    }
} 
