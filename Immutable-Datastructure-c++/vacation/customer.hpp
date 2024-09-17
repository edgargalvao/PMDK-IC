#pragma once

#include <immer/map.hpp>
#include <reservation.hpp>
#include <immer/memory_policy.hpp>
#include <immer/nvm_utils.hpp>

// TODO: change to a immer list?
//using LIST_T = immer::vector<ReservationInfo*>;
// Unlike original vacation, we store reservation info in a map
// ReservationType_id : price (eg: 0_10120: 10000).
using LIST_T = immer::map<std::string, long>;

class Customer {
  public:

      void* operator new(size_t size) {
          return immer::default_memory_policy::heap::type::allocate(size);
      }

      void operator delete(void* data) {
          immer::default_memory_policy::heap::type::deallocate(0, data);
      }

      void operator delete(void* data, size_t size) {
          immer::default_memory_policy::heap::type::deallocate(size, data);
      }

    Customer (long id, LIST_T *reservation_info_list)
      : id_(id), reservation_info_list_(reservation_info_list) {}

    long GetId() const { return id_; }

    Customer* AddReservationInfo(ReservationType type,
                                 long id, long price) const;

    Customer* RemoveReservationInfo(ReservationType type,
                                    long id) const;

    long GetBill() const;

    const LIST_T* GetReservationInfoList() const {
      return reservation_info_list_;
    }

  private:
//    const char *nvm_id_;
    long id_;
    LIST_T *reservation_info_list_;
};

Customer* AllocCustomer(long id, LIST_T *reservation_info_list);

// TODO: No idea how this should work. Maybe return nullptr?
Customer* FreeCustomer(Customer *customer);
