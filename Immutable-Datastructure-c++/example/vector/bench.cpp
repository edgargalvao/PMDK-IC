//
// immer: immutable data structures for C++
// Copyright (C) 2016, 2017, 2018 Juan Pedro Bolivar Puente
//
// This software is distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://boost.org/LICENSE_1_0.txt
//

#include <chrono> 
#include <cassert>
#include <string>
#include <immer/vector.hpp>
#include <immer/vector_transient.hpp>
#include <iostream>
#include <random>

using namespace std;
using vector_type = immer::vector<uint64_t>;

void PopulateKeysAndOps(int num_ops, int *keys) {
    std::random_device device;
    int random_seed = device();
    std::mt19937 generator(random_seed);
    std::uniform_int_distribution<int> 
        distribution(0, num_ops);

    for (int i = 0; i < num_ops; i++) {
        keys[i] = i;
    }
    std::shuffle(keys, keys+num_ops, generator);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "Error! Correct Usage: ./vector_nvm <backing-file> ops\n";
        return 0;
    }

    nvm_initialize(argv[1], 0);

    int num_ops = atoi(argv[2]);
    std::chrono::time_point<std::chrono::high_resolution_clock> start, stop;
    auto* keys = new int [num_ops];
    PopulateKeysAndOps(num_ops, keys);

    auto* immer_vector = new vector_type(num_ops, 0);
    auto* immer_vector2 = new vector_type(num_ops, 0);

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_ops; i++) {
        immer_vector = immer_vector->set_ptr(i, i);
    }
    stop = std::chrono::high_resolution_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>
        (stop-start).count();
    cout << "Latency: " << latency << " ns\n";
    cout << "Vector Throughput: " << (num_ops*1E9/latency) << " op/s\n";

    auto immer_vector_trans = immer_vector2->transient();
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_ops; i++) {
        immer_vector_trans.set(i, i);
    }
    stop = std::chrono::high_resolution_clock::now();
    latency = std::chrono::duration_cast<std::chrono::nanoseconds>
        (stop-start).count();
    cout << "Latency: " << latency << " ns\n";
    cout << "Transient Vector Throughput: " << (num_ops*1E9/latency) << " op/s\n";
    return 1;
} 
