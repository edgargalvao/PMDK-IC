//
// immer: immutable data structures for C++
// Copyright (C) 2016, 2017, 2018 Juan Pedro Bolivar Puente
//
// This software is distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://boost.org/LICENSE_1_0.txt
//

#include <immer/box.hpp>
#include <cassert>
#include <string>
#include <cstring>
#include <nvm_malloc.h>
#include <iostream>

using box_type = immer::box<std::string>;

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
        box_type* v1 = new (mem) box_type{"hello"};
        nvm_persist(mem, sizeof(box_type));
        nvm_activate_id("box");
        box_type *v2 = v1->update_ptr([&] (auto l) {
            return l + ", world!";
        });

        std::cout << "Original Value:" << v1->get() << std::endl;
        std::cout << "Updated Value:" << v2->get() << std::endl;
    } else {
        // recovered
        nvm_initialize("/dev/shm/box/", 1); 
        box_type* box_recovered = (box_type*) nvm_get_id("box");
        std::cout << "Box recovered at:" << (void*) box_recovered << std::endl;
        std::cout << "Recovered value:" << box_recovered->get() << std::endl;        
    }
} 
