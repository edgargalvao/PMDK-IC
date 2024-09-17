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

using map_type = immer::map<int, int>;

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
        auto* v0 = new (mem) map_type();
        NVM_PERSIST(mem, sizeof(map_type));
        nvm_activate_id("map1");
        int i = 0;
        map_type *v1 = std::move(v0);
        for (; i < 20000; i++) {
            v1 = v1->set_ptr(i, i);
            std::cout << "i:" << i << std::endl;
            if (i == 19999) {
                std::cout << "Updated size:" << v1->size() << std::endl;
                std::cout << i << ":" << *v1->find(i) << std::endl;
            }
        }
    } else {
        // recovered
        nvm_initialize("/dev/shm/map/", 1);
        map_type* map_recovered = (map_type*) nvm_get_id("map1");
        std::cout << "map recovered at:" << (void*) map_recovered << std::endl;
        const auto size = map_recovered->size();
        std::cout << "Size:" << size << std::endl;
        for (int i = 0; (unsigned int) i < size; i++) {
            assert(*map_recovered->find(i) == i);
        }
        std::cout << (size-1) << ":" << *map_recovered->find(size-1) << std::endl;
    }
} 
