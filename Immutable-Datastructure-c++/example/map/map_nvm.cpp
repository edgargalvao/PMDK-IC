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
    if (argc < 3) {
        std::cout << "Error! Correct Usage: ./map_nvm <backing-file> <1/0>, 1 for recovery\n";
        return 0;
    }
    const char* path = argv[1];
    int option = atoi(argv[2]);
    if (option == 0) {
        nvm_initialize(path, 0);
        map_type *mem = (map_type *) nvm_reserve_id("map1", sizeof(map_type));
        std::cout << "map located at:" << (void*) mem << std::endl;
        const auto* v0 = new (mem) map_type();
        nvm_persist(mem, sizeof(map_type));
        nvm_activate_id("map1");
        const auto* v1 = v0->set_ptr("hello", 42);
        std::cout << "Updated map located at " << (void *) v1 << std::endl;
        std::cout << "Original size:" << v0->size() << std::endl;
        std::cout << "Updated size:" << v1->size() << std::endl;
        std::cout << "Hello:" << *v1->find("hello") << std::endl;
    } else {
        // recovered
        nvm_initialize(path, 1);
        map_type* map_recovered = (map_type*) nvm_get_id("map1");
        std::cout << "map recovered at:" << (void*) map_recovered << std::endl;
        std::cout << "Size:" << map_recovered->size() << std::endl;
        std::cout << "Hello:" << *map_recovered->find("hello") << std::endl;
        map_recovered = map_recovered->set_ptr("world", 41);
        std::cout << "Size:" << map_recovered->size() << std::endl;
        std::cout << "World:" << *map_recovered->find("world") << std::endl;
    }
} 
