//
// immer: immutable data structures for C++
// Copyright (C) 2016, 2017, 2018 Juan Pedro Bolivar Puente
//
// This software is distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://boost.org/LICENSE_1_0.txt
//

#include <cassert>
#include <string>
#include <array>
#include <immer/set.hpp>
#include <cstring>
#include <nvm_malloc.h>
#include <iostream>


using set_type = immer::set<int>;

struct setHolder{
    set_type *set;
};

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cout << "Error! Correct Usage: ./set_nvm <path> <1/0>, 1 for recovery\n";
        return 0;
    }
    const char *path = argv[1];
    int option = atoi(argv[2]);
    if (option == 0) {
        nvm_initialize(path, 0);
        auto *mem = (setHolder*) nvm_reserve_id("set", sizeof(setHolder));
        auto* set_holder = new (mem) setHolder();
        set_holder->set = new set_type();
        NVM_PERSIST(mem, sizeof(setHolder));
        nvm_activate_id("set");
        int i = 0;
        for (; i < 4; i++) {
            auto* v1 = set_holder->set->insert_ptr(i);
            std::cout << "i:" << i << std::endl;
            if (i > 0) {
                std::cout << "Updated size:" << v1->size() << std::endl;
            }
            set_holder->set = v1;
        }
    } else {
        // recovered
        nvm_initialize(path, 1);
        auto* set_recovered = (setHolder*) nvm_get_id("set");
        const auto size = set_recovered->set->size();
        std::cout << "set recovered at:" << (void*) set_recovered << std::endl;
        std::cout << "Size:" << size << std::endl;
        auto count = (set_recovered->set->count((int)size-1));
        std::cout << (size-1) << " count:" << count << std::endl;
        //std::cout << *ptr << std::endl;
        auto* v1 = set_recovered->set->insert_ptr(size);
        set_recovered->set = v1;
    }
} 
