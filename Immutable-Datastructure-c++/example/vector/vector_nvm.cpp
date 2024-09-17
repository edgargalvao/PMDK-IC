//
// immer: immutable data structures for C++
// Copyright (C) 2016, 2017, 2018 Juan Pedro Bolivar Puente
//
// This software is distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://boost.org/LICENSE_1_0.txt
//

#include <cassert>
#include <string>
#include <immer/vector.hpp>
#include <cstring>
#include <iostream>

using vector_type = immer::vector<int>;

struct VectorHolder{
    vector_type *vector;
};

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cout << "Error! Correct Usage: ./vector_nvm <backing-file> <1/0>, 1 for recovery\n";
        return 0;
    }
    const char* path = argv[1];
    int option = atoi(argv[2]);
    if (option == 0) {
        nvm_initialize(path, 0);
        auto *mem = (VectorHolder*) nvm_reserve_id("Vector", sizeof(VectorHolder));
        auto* vector_holder = new (mem) VectorHolder();
        std::cout << "Vector located at:" << (void*) mem << std::endl;
        vector_holder->vector = new vector_type(1, 0);
        _mm_sfence();
        NVM_PERSIST(vector_holder, 1);
        nvm_activate_id("Vector");
        auto* v0 = vector_holder->vector;
        auto* v1 = v0->push_back_ptr(1);
        _mm_sfence();
        std::cout << "Updated vector located at " <<  (void*) v1 << std::endl;
        std::cout << "Original size:" << v0->size() << std::endl;
        std::cout << "Updated size:" << v1->size() << std::endl;
        std::cout << "Vector1[0]:" << (*v1)[0]<< std::endl;
        vector_holder->vector = v1;
    } else {
        // recovered
        nvm_initialize(path, 1);
        VectorHolder *vector_holder = (VectorHolder*) nvm_get_id("Vector");
        std::cout << "vector recovered at:" << (void*) vector_holder << std::endl;
        const auto* v0 = vector_holder->vector;
        std::cout << "Size:" << v0->size() << std::endl;
        for (unsigned int i=0; i < v0->size(); i++) {
            std::cout << "Vector[" << i << "]:" << (*v0)[i]<< std::endl;
        }
        const auto size = v0->size();
        vector_holder->vector = v0->push_back_ptr(size);
        std::cout << "Updated Size:" << vector_holder->vector->size() << std::endl;
        _mm_sfence();
    }
    return 1;
} 
