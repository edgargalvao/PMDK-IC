//
// immer: immutable data structures for C++
// Copyright (C) 2016, 2017, 2018 Juan Pedro Bolivar Puente
//
// This software is distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://boost.org/LICENSE_1_0.txt
//

#include <cassert>
#include <string>
#include <immer/queue.hpp>
#include <cstring>
#include <iostream>

using queue_type = immer::queue<int>;

struct QueueHolder{
    queue_type *queue;
};

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cout << "Error! Correct Usage: ./queue_nvm <backing-file> <1/0>, 1 for recovery\n";
        return 0;
    }
    const char* path = argv[1];
    int option = atoi(argv[2]);
    if (option == 0) {
        nvm_initialize(path, 0);
        auto *mem = (QueueHolder*) nvm_reserve_id("queue", sizeof(QueueHolder));
        auto* queue_holder = new (mem) QueueHolder();
        std::cout << "queue located at:" << (void*) mem << std::endl;
        queue_holder->queue = new queue_type();
        _mm_sfence();
        NVM_PERSIST(queue_holder, 1);
        nvm_activate_id("queue");
        auto* v0 = queue_holder->queue;
        auto* v1 = v0->push_back_ptr(1);
        auto* v2 = v1->push_back_ptr(1);
        _mm_sfence();
        std::cout << "Updated queue located at " <<  (void*) v1 << std::endl;
        std::cout << "Original queue:";
        v0->printq();
        std::cout << "Updated queue:";
        v1->printq();
        std::cout << "Updated queue:";
        v2->printq();
        queue_holder->queue = v2;
    } else {
        // recovered
        nvm_initialize(path, 1);
        QueueHolder *queue_holder = (QueueHolder*) nvm_get_id("queue");
        std::cout << "queue recovered at:" << (void*) queue_holder << std::endl;
        const auto* v0 = queue_holder->queue;
        queue_holder->queue = v0->push_back_ptr(1);
        std::cout << "Original queue:";
        v0->printq();
        std::cout << "Updated queue:"; 
        queue_holder->queue->printq();
        _mm_sfence();
    }
    return 1;
}
