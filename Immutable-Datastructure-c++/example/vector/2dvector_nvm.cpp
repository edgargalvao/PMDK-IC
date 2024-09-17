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
using twodim_vector_type = immer::vector<vector_type*>;

struct VectorHolder{
    twodim_vector_type *vector;
};

void PrintTwoDimVector(twodim_vector_type* v) {
    for (unsigned int i = 0; i < v->size(); i++) {
        vector_type* v_in = (*v)[i];
        for (unsigned int j = 0; j < v_in->size(); j++) {
            std::cout << (*v_in)[j] << " "; 
        }
        std::cout << std::endl;
    }
}

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
        vector_holder->vector = new twodim_vector_type();
        _mm_sfence();
        NVM_PERSIST(vector_holder, 1);
        nvm_activate_id("Vector");
        for (int i = 0; i < 4; i++) {
            auto* v0 = new vector_type(4, 1);
            std::cout << "New vector @:" << (void*) v0 << std::endl;
            vector_holder->vector = vector_holder->vector->push_back_ptr(v0);
        }
        _mm_sfence();
        std::cout << "Vector.size()" << vector_holder->vector->size() << std::endl;
        PrintTwoDimVector(vector_holder->vector);
    } else {
        // recovered
        nvm_initialize(path, 1);
        VectorHolder *vector_holder = (VectorHolder*) nvm_get_id("Vector");
        const auto* v0 = vector_holder->vector;
        std::cout << "Size:" << v0->size() << std::endl;
        PrintTwoDimVector(vector_holder->vector);
        for (unsigned int i = 0; i < vector_holder->vector->size(); i++) {
            vector_type* v_in = (*vector_holder->vector)[i];
            v_in = v_in->push_back_ptr(1);
            vector_holder->vector = vector_holder->vector->set_ptr(i, v_in);
        }
    }
    return 1;
} 
