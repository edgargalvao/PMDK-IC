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

class PM_String {
  public:
    PM_String(const char *data_in) {
        const auto data_len = strlen(data_in);
        char *temp_data = (char *) nvm_reserve_id(data_in, data_len);
        std::cout << "Allocating id:" << data_in << std::endl; 
        std::cout << "Temp data allocated at:" << static_cast<void*>(temp_data) << std::endl;
        memcpy(temp_data, data_in, data_len);
        nvm_persist(temp_data, data_len);
        nvm_activate_id(data_in);
        data = temp_data;
    }

    bool operator==(const PM_String& other) const {
        return std::string(other.data) == std::string(data);
    }

    PM_String operator+(const PM_String& other) const {
        const auto addition = (std::string(data) + std::string(other.data));
        std::cout << "Addition:" << addition << std::endl;
        return PM_String(addition.c_str());
    }
    
    friend std::ostream& operator<< (std::ostream& stream, const PM_String& value) {
            stream << value.data;
            return stream;
    }

    size_t size() {
        return strlen(data);
    }

    const char *data;
};

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "Error! Correct Usage: ./simple_ctr <1/0>, 1 for recovery\n";
        return 0;
    }
    int option = atoi(argv[1]);
    if (option == 0) {
        nvm_initialize("/dev/shm/box/", 0);
        auto v1 = immer::box<PM_String>{"hello"};
        auto v2 = v1.update([&] (auto l) {
            return l + ", world!";
        });
        assert((v1 != immer::box<PM_String>{"hello2"}));
        assert((v2 != immer::box<PM_String>{"hello, world!2"}));
    } else {
        // recovered
        nvm_initialize("/dev/shm/box/", 1); 
        std::cout << "Recovered address:" << nvm_get_id("hello, world!") << std::endl;
        immer::box<PM_String> box_recovered = immer::box<PM_String>{(const char *)nvm_get_id("hello, world!")};
        std::cout << "Recovered value:" << box_recovered.get() << std::endl;        
    }
} 
