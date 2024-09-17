#pragma once

#include <iostream>

#include <immer/map.hpp>
#include <reservation.hpp>
#include <customer.hpp>

using MAP_T = immer::map<long, Reservation*>;
using CUSTOMER_MAP_T = immer::map<long, Customer*>;

class Manager {
  public:
    Manager(MAP_T *car_table_ptr, MAP_T *room_table_ptr,
            MAP_T *flight_table_ptr, CUSTOMER_MAP_T *customer_table_ptr)
      : car_table_ptr_ (car_table_ptr),
        room_table_ptr_ (room_table_ptr),
        flight_table_ptr_ (flight_table_ptr),
        customer_table_ptr_ (customer_table_ptr)
        {} 

    // Manager(Manager *other) : car_table_ptr_(other->car_table_ptr_) {}

    Manager* AddCar(long id, long num, long price) const;
    Manager* AddRoom(long id, long num, long price) const;
    Manager* AddFlight(long id, long num, long price) const;

    Manager* DeleteCar(long id, long num) const;
    Manager* DeleteRoom(long id, long num) const;
    Manager* DeleteFlight(long id);

    // Add/DeleteCustomer is not marked const as we may return this 
    // if no customer is deleted. Using a clever combination of const 
    // in the return path, we may be able to make it work.
    // TODO: Mark as const.
    Manager* AddCustomer(long id);   

    Manager* DeleteCustomer(long id);

    long QueryCustomerBill(long id) const;

    long QueryCar (long id) {
      auto* reservation = car_table_ptr_->find(id);
      if (reservation == nullptr)
        return -1;
      return (*reservation)->num_free;
    }

    // Returns -1 if does not exist
    long QueryCarPrice (long id) {
      auto* reservation = car_table_ptr_->find(id);
      if (reservation == nullptr)
        return -1;
      return (*reservation)->price;
    }

    long QueryRoom (long id) {
      auto* reservation = room_table_ptr_->find(id);
      if (reservation == nullptr)
        return -1;
      return (*reservation)->num_free;
    }

    // Returns -1 if does not exist
    long QueryRoomPrice (long id) {
      auto* reservation = room_table_ptr_->find(id);
      if (reservation == nullptr)
        return -1;
      return (*reservation)->price;
    }

    long QueryFlight (long id) {
      auto* reservation = flight_table_ptr_->find(id);
      if (reservation == nullptr)
        return -1;
      return (*reservation)->num_free;
    }

    // Returns -1 if does not exist
    long QueryFlightPrice (long id) {
      auto* reservation = flight_table_ptr_->find(id);
      if (reservation == nullptr)
        return -1;
      return (*reservation)->price;
    }

    Manager* ReserveCar(long customer_id, long id);
    
    Manager* ReserveRoom(long customer_id, long id);
    
    Manager* ReserveFlight(long customer_id, long id);

    static long NumReservationTypes() { return num_tables_; }  

    const MAP_T* GetCarTablePtr() const { return car_table_ptr_; }
    const MAP_T* GetRoomTablePtr() const { return room_table_ptr_; }
    const MAP_T* GetFlightTablePtr() const { return flight_table_ptr_; }

    void AssertInnerTablesNotNull() const {
        assert (car_table_ptr_ != nullptr);
        assert (room_table_ptr_ != nullptr);
        assert (flight_table_ptr_ != nullptr);
        assert (customer_table_ptr_ != nullptr);
    }
    
    void PrintTables() const;

  private:

    static const int num_tables_ = 3;
    MAP_T *car_table_ptr_;
    MAP_T* room_table_ptr_;
    MAP_T* flight_table_ptr_;
    CUSTOMER_MAP_T *customer_table_ptr_;
};

Manager* AllocManager(MAP_T *car_table_ptr, MAP_T *room_table_ptr,
                      MAP_T *flight_table_ptr, CUSTOMER_MAP_T *customer_table_ptr);


inline MAP_T* DeleteReservation(MAP_T *table_ptr, long id) {
    return table_ptr->erase_ptr(id);
}


inline MAP_T* AddReservation(MAP_T* table_ptr, long id,
        long num, long price) {

    // Find if table contains the id:
    auto* const old_reservation = table_ptr->find(id);
    long num_used = 0, num_free = 0, num_total = 0; 
    if (old_reservation == NULL) {
        if (num < 1 || price < 0)
            return table_ptr;
        // We create new reservation after the if-else block.
        num_free = num;
        num_total = num;
    }
    else {
        // Add number to total
        if ((*old_reservation)->num_free + num < 0) {
            // Can't delete more than available.
            return table_ptr;
        }
        num_used = (*old_reservation)->num_used;
        num_free = (*old_reservation)->num_free + num;
        num_total = (*old_reservation)->num_total + num;
        price = (*old_reservation)->price;
        if (num_total == 0) {
            // TODO: Delete resource even if it may be in use?
            return DeleteReservation(table_ptr, id);
        }
    }

    // Allocate reservation in persistent memory with updated num and price.
    auto* reservation = AllocReservation(id, num_used, num_free, num_total, price);
    // set_ptr updates the value of id.
    return table_ptr->set_ptr(id, reservation);
}

inline CUSTOMER_MAP_T* AddCustomer (CUSTOMER_MAP_T *customer_table_ptr,
        long id) {
    auto* customer = customer_table_ptr->find(id);

    if (customer != nullptr) 
        return customer_table_ptr;

    // Allocate customer in persistent memory.
    auto* new_customer = AllocCustomer(id, nullptr);

    // Insert into the map
    return customer_table_ptr->set_ptr(id, new_customer);
}
