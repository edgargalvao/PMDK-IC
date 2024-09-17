#include <string>
#include <sstream>

#include <nvm_malloc.h>
#include <util.hpp>
#include <customer.hpp>
#include <constants.hpp>

std::string ToKeyString (ReservationType type, long id) {
    std::stringstream ss;
    ss << type << "_" << id;
    return ss.str();
}

Customer* AllocCustomer(long id, LIST_T *reservation_info_list) {
    // Allocate the reservation list, if NULL.
    if (reservation_info_list == nullptr) {
        reservation_info_list = new LIST_T();
    }

    Customer *customer_ptr = new Customer(id, reservation_info_list);
    LOG("Customer allocated at %p", (void*) customer_ptr);

    // TODO: maybe I should use nvm_activate here?
    // I do not want to as manager_ptr itself is not activated yet.
    NVM_PERSIST_NOW(customer_ptr, sizeof(Customer));
    // nvm_activate_id(nvm_id);
    return customer_ptr;
}

// Returns nullptr if duplicate info.
Customer *Customer::AddReservationInfo(ReservationType type,
                                       long id, long price) const
{
    // Create new Reservation Info.
    /*
    auto *reservation_info = AllocReservationInfo(type, id, price);
    assert(reservation_info != nullptr);
    */
    auto key = ToKeyString(type, id);
    
    // Ensure not a duplicate entry
    auto* const reservation_info = reservation_info_list_->find(key);
    if (reservation_info != nullptr)
        return nullptr;

    // Insert into list.

    auto *reservation_info_list = 
        reservation_info_list_->set_ptr(key, price);

    return AllocCustomer(id_, reservation_info_list);
}

Customer *Customer::RemoveReservationInfo(ReservationType type,
                                          long id) const
{
    auto *reservation_info_list = 
        reservation_info_list_->erase_ptr(ToKeyString(type, id));
    return AllocCustomer(id_, reservation_info_list);
}

long Customer::GetBill() const {
    long bill = 0;
    auto iterator = reservation_info_list_->begin();
    while (iterator != reservation_info_list_->end()) {
        const auto reservation_info = *iterator;
        // Price is the value in our key-value store.
        bill += reservation_info.second;
        iterator++;
    }
    return bill;
}
