//
// immer: immutable data structures for C++
// Copyright (C) 2016, 2017, 2018 Juan Pedro Bolivar Puente
//
// This software is distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://boost.org/LICENSE_1_0.txt
//

#include <cassert>
#include <string>
#include <immer/map.hpp>
#include <cstring>
#include <nvm_malloc.h>
#include <iostream>

using map_type = immer::map<std::string, int>;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "Error! Correct Usage: ./map_nvm <1/0>, 1 for recovery\n";
        return 0;
    }
    int option = atoi(argv[1]);
    if (option == 0) {
        nvm_initialize("/dev/shm/map/", 0);
        map_type *mem = (map_type *) nvm_reserve_id("map1", sizeof(map_type));
        std::cout << "map located at:" << (void*) mem << std::endl;
        const auto* v0 = new (mem) map_type();
        NVM_PERSIST(mem, sizeof(map_type));
        nvm_activate_id("map1");
        int i = 0;
        for (; i < 200; i++) {
            const auto* v1 = v0->set_ptr("hello", i);
            std::cout << "i:" << i << std::endl;
            if (i == 199) {
                std::cout << "Updated size:" << v1->size() << std::endl;
                std::cout << "Hello:" << *v1->find("hello") << std::endl;
            }
        }
    } else {
        // recovered
        nvm_initialize("/dev/shm/map/", 1);
        map_type* map_recovered = (map_type*) nvm_get_id("map1");
        std::cout << "map recovered at:" << (void*) map_recovered << std::endl;
        std::cout << "Size:" << map_recovered->size() << std::endl;
        std::cout << "Hello:" << *map_recovered->find("hello") << std::endl;
    }
} 
