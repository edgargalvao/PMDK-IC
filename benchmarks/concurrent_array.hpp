#pragma once

#include <mutex> // For std::unique_lock
#include <shared_mutex>


class concurrent_array
{
  public: 

    concurrent_array() {
        dummy_array = new int[1000000];
    }

    void set(uint64_t index, int value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        //dummy_array[index] = value;
    }

    int get(uint64_t index) {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return -1;
        //return dummy_array[index]; 
    }

  private:
      mutable std::shared_mutex mutex_;
      int *dummy_array;
};
