#include <iostream>
#include <cassert>
#include <stdlib.h>

#include <nvm_malloc.h>
#include <util.hpp>
#include <manager.hpp>
#include <constants.hpp>

void PrintMapContents(const MAP_T *map, std::string map_name) {
    auto iterator = map->begin();
    long num_used = 0;
    long num_free = 0;
    long num_total = 0;
    std::cout << map_name << " Size:" << map->size() << std::endl;
    while (iterator != map->end()) {
        auto *reservation = (*iterator).second;
        //LOG("Id:%ld Reservation:%s", (*iterator).first,
        //                             ToString(reservation).c_str());
        
        num_used += reservation->num_used;
        num_free += reservation->num_free;
        num_total += reservation->num_total;;
        iterator++;
    }
    std::cout << "Free: " << num_free << " Used: " << num_used
              << " Total: " << num_total << std::endl;
}

Manager* AllocManager(MAP_T *car_table_ptr, MAP_T *room_table_ptr,
                      MAP_T *flight_table_ptr,
                      CUSTOMER_MAP_T *customer_table_ptr) {
    // Allocate each of the reservation tables, if NULL.
    if (car_table_ptr == nullptr) {
        car_table_ptr = new MAP_T();
    } 
    if (room_table_ptr == nullptr) {
        room_table_ptr = new MAP_T();
    }
    if (flight_table_ptr == nullptr) {
        flight_table_ptr = new MAP_T();
    } 
    if (customer_table_ptr == nullptr) {
        customer_table_ptr = new CUSTOMER_MAP_T();
    }

    void *mem = nvm_reserve(sizeof(Manager));
    assert (mem != NULL);
    //LOG("Manager allocated at %p", mem);
    Manager *manager_ptr = new (mem) Manager(car_table_ptr, room_table_ptr,
                                             flight_table_ptr,
                                             customer_table_ptr);
    
    // TODO: maybe I should use nvm_activate here for all table ptrs?
    // I do not want to as manager_ptr itself is not activated yet.
    NVM_PERSIST_NOW(manager_ptr, sizeof(Manager));
    return manager_ptr;
}

MAP_T* CancelReservation(MAP_T* table_ptr, long id) {
    auto* const reservation = table_ptr->find(id);
    if (reservation == nullptr)
        return table_ptr;
    
    Reservation* new_reservation = CancelReservation(*reservation);
    if (new_reservation == nullptr)
        return table_ptr;
     
    table_ptr = table_ptr->set_ptr(id, new_reservation);
    return table_ptr;
} 

Manager* Manager::AddCar(long id, long num, long price) const {
    return AllocManager(AddReservation(car_table_ptr_, id, num, price),
                        room_table_ptr_, flight_table_ptr_,
                        customer_table_ptr_);
}

Manager* Manager::AddRoom(long id, long num, long price) const {
    return AllocManager(car_table_ptr_,
                        AddReservation(room_table_ptr_, id, num, price),
                        flight_table_ptr_, customer_table_ptr_);
}

Manager* Manager::AddFlight(long id, long num, long price) const {
    return AllocManager(car_table_ptr_, room_table_ptr_, 
                        AddReservation(flight_table_ptr_, id, num, price),
                        customer_table_ptr_);
}

Manager* Manager::DeleteCar(long id, long num) const {
    return AllocManager(AddReservation(car_table_ptr_, id, -num ,-1),
                        room_table_ptr_, flight_table_ptr_,
                        customer_table_ptr_);
}

Manager* Manager::DeleteRoom(long id, long num) const {
    return AllocManager(car_table_ptr_,
                        AddReservation(room_table_ptr_, id, -num, -1),
                        flight_table_ptr_, customer_table_ptr_);
}

/* Fails if a customer has reservations on this flight */
Manager* Manager::DeleteFlight(long id) {
    auto* const reservation = flight_table_ptr_->find(id);
    if (reservation == nullptr || (*reservation)->num_used > 0)
        return this;
    return AllocManager(car_table_ptr_, room_table_ptr_,
                        DeleteReservation(flight_table_ptr_, id),
                        customer_table_ptr_);
}

Manager* Manager::ReserveCar(long customer_id, long id) {
    auto* customer_ptr = customer_table_ptr_->find(customer_id);
    Customer* customer;
    if (customer_ptr == nullptr) {
        return this;
    } else {
        customer = *customer_ptr;
    }

    auto* const reservation = car_table_ptr_->find(id);
    if (reservation == nullptr)
        return this;
    
    Reservation* new_reservation = MakeReservation(*reservation);
    if (new_reservation == nullptr)
        return this;
    auto* new_car_table_ptr = car_table_ptr_->set_ptr(id, new_reservation);
    customer = customer->AddReservationInfo(
        RESERVATION_CAR, id, new_reservation->price);
    if (customer == nullptr) {
        // Original vacation uses list with flag LIST_NO_DUPLICATES
        // which does not allow duplicates and returns FALSE here.
        return this; 
    }
    auto* new_customer_table_ptr = 
        customer_table_ptr_->set_ptr(customer_id, customer);

    return AllocManager(new_car_table_ptr, room_table_ptr_,
                        flight_table_ptr_, new_customer_table_ptr);
}

Manager* Manager::ReserveRoom(long customer_id, long id) {
    auto* customer_ptr = customer_table_ptr_->find(customer_id);
    Customer* customer;
    if (customer_ptr == nullptr) {
        return this;
    } else {
        customer = *customer_ptr;
    }

    auto* const reservation = room_table_ptr_->find(id);
    if (reservation == nullptr)
        return this;
    
    Reservation* new_reservation = MakeReservation(*reservation);
    if (new_reservation == nullptr)
        return this;
    auto* new_room_table_ptr = room_table_ptr_->set_ptr(id, new_reservation);
    customer = customer->AddReservationInfo(
        RESERVATION_ROOM, id, new_reservation->price);
    if (customer == nullptr) {
        // Original vacation uses list with flag LIST_NO_DUPLICATES
        // which does not allow duplicates and returns FALSE here.
        return this; 
    }
    auto* new_customer_table_ptr = 
        customer_table_ptr_->set_ptr(customer_id, customer);

    return AllocManager(car_table_ptr_, new_room_table_ptr, 
                        flight_table_ptr_, new_customer_table_ptr);
}

Manager* Manager::ReserveFlight(long customer_id, long id) {
    auto* customer_ptr = customer_table_ptr_->find(customer_id);
    Customer* customer;
    if (customer_ptr == nullptr) {
        // Allocate customer in persistent memory.
        return this;
    } else {
        customer = *customer_ptr;
    }

    auto* const reservation = flight_table_ptr_->find(id);
    if (reservation == nullptr)
        return this;
    
    Reservation* new_reservation = MakeReservation(*reservation);
    if (new_reservation == nullptr)
        return this;
    auto* new_flight_table_ptr = flight_table_ptr_->set_ptr(id, new_reservation);
    customer = customer->AddReservationInfo(
        RESERVATION_FLIGHT, id, new_reservation->price);
    if (customer == nullptr) {
        // Original vacation uses list with flag LIST_NO_DUPLICATES
        // which does not allow duplicates and returns FALSE here.
        return this; 
    }
    auto* new_customer_table_ptr = 
        customer_table_ptr_->set_ptr(customer_id, customer);

    return AllocManager(car_table_ptr_, room_table_ptr_, 
                        new_flight_table_ptr, new_customer_table_ptr);
}

Manager* Manager::AddCustomer(long id) {
    return AllocManager(car_table_ptr_, room_table_ptr_,
            flight_table_ptr_, ::AddCustomer(customer_table_ptr_, id));
}

Manager* Manager::DeleteCustomer(long id) {
    auto* customer = customer_table_ptr_->find(id);
    if (customer == nullptr) 
        return this;

    // TODO: Cancel all reservations from this customer
    auto* car_table_ptr = car_table_ptr_;
    auto* room_table_ptr = room_table_ptr_;
    auto* flight_table_ptr = flight_table_ptr_;
    assert (num_tables_ == 3); // Failsafe to make sure I add code here.

    auto* reservation_list = (*customer)->GetReservationInfoList();
    NOTE("Customer:%lu has %lu reservations\n", id, reservation_list->size());
    auto iterator = reservation_list->begin();
    while (iterator != reservation_list->end()) {
        const auto reservation_info = *iterator;
        // Key is type_id.
        // TODO: move to customer.h
        const auto reservation_type = reservation_info.first[0]-'0';
        const auto id = atoi(
            reservation_info.first.substr(2,std::string::npos).c_str());
        switch (reservation_type) {
            case 0: 
                // Returns same map if id not found.
                car_table_ptr = CancelReservation(car_table_ptr, id);
                break;
            case 1:
                flight_table_ptr = CancelReservation(flight_table_ptr, id);
                break;
            case 2:
                room_table_ptr = CancelReservation(room_table_ptr, id);
                break;
            default:
                std::cerr << "Reservation type not supported:" 
                          << reservation_type << std::endl;
        }
        iterator++;
        // TODO: Deallocate reservation_info
    }


    // Allocate customer in persistent memory.
    auto* new_customer = AllocCustomer(id, nullptr);

    // Original vacation deletes customer from customer table here, 
    // but WHISPER version doesn't.
    return AllocManager(car_table_ptr, room_table_ptr, flight_table_ptr, 
                        customer_table_ptr_->set_ptr(id, new_customer));
}

long Manager::QueryCustomerBill(long id) const {
    long bill = -1;
    auto* customer = *(customer_table_ptr_->find(id));
    if (customer != nullptr) 
        bill = customer->GetBill();
    return bill;
}

void Manager::PrintTables() const {
    PrintMapContents(car_table_ptr_, "car table");
    PrintMapContents(room_table_ptr_, "room table");
    PrintMapContents(flight_table_ptr_, "flight table");
}
