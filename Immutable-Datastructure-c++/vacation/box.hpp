#pragma once

#include <iostream>
#include <mutex>
#include <immer/memory_policy.hpp>
#include <manager.hpp>

class Box {
  public:
    Box(Manager *manager_ptr) : manager_lock_(),
         manager_ptr_(manager_ptr)
        {} 

    void ResetMutex() {
        // We want to ensure mutex is locked before we try to unlock it.
        //manager_lock_.try_lock();
        //manager_lock_.unlock();
    }

    // Can modify the pointer itself which doesn't affect us,
    // but cannot modify the underlying manager.
    const Manager* GetManager() {
        return manager_ptr_;
    }

    Manager* LockAndReturnManager() {
        //manager_lock_.lock();
        return manager_ptr_;
    }

    void UpdateManagerAndUnlock(Manager *new_manager) {
        // TODO: This may result in extra SFENCE when manager is not updated as well?
        NVM_SFENCE();
        manager_ptr_ = new_manager;
        //manager_lock_.unlock();
    }
    //Manager_t *acquire_read_lock()

  private:
    // int readers_;
    // bool lock_held_;
    std::mutex manager_lock_;
    Manager *manager_ptr_;
};
