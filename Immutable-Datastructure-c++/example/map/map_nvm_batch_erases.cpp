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
    if (argc < 3) {
        std::cout << "Error! Correct Usage: ./map_nvm <backing-file> <1/0>, 1 for recovery\n";
        return 0;
    }
    const char *path = argv[1];
    int option = atoi(argv[2]);
    if (option == 0) {
        nvm_initialize(path, 0);
        map_type *mem = (map_type *) nvm_reserve_id("map1", sizeof(map_type));
        std::cout << "map located at:" << (void*) mem << std::endl;
        auto* v0 = new (mem) map_type();
        NVM_PERSIST(mem, sizeof(map_type));
        nvm_activate_id("map1");
        int i = 0;
        map_type *v1 = std::move(v0);
        for (; i < 10000; i++) {
            v1 = v1->set_ptr(i, i);
            std::cout << "i:" << i << std::endl;
        }
        // Start erasing elements from the bottom
        for (i = 0; i<10000; i++) {
            v1 = v1->erase_ptr(i);
        }
        std::cout << "Updated size:" << v1->size() << std::endl;
    } else {
        // recovered
        nvm_initialize(path, 1);
        map_type* map_recovered = (map_type*) nvm_get_id("map1");
        std::cout << "map recovered at:" << (void*) map_recovered << std::endl;
        const auto size = map_recovered->size();
        std::cout << "Size:" << size << std::endl;
        bool first = true;
        for (int i = 0; (unsigned int) i < size; i++) {
            const auto* value_ptr = map_recovered->find(i);
            if(value_ptr == nullptr && first) {
                // erased?
                continue;
            }
            if (first) {
                std::cout << "Erased till " << i << std::endl;
                first = false;
            }
            assert(*value_ptr == i);
        }
    }
} 
