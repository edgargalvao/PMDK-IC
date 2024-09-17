#include <cassert>
#include <sstream>

#include <nvm_malloc.h>
#include <reservation.hpp>

// Here id is car/flight/room id depending on type
Reservation* AllocReservation(long id, long num_used, long num_free,
                              long num_total, long price) {
    // nvm_utils overloads new operator.
    auto *reservation = new Reservation(id, num_used, num_free, num_total, price);
    NVM_PERSIST_NOW(reservation, sizeof(ReservationInfo));
    return reservation;
}

// Here id is car/flight/room id depending on type
ReservationInfo* AllocReservationInfo(ReservationType type, long id,
                                      long price) {
    // nvm_utils overloads new operator.
    auto *reservation_info = new ReservationInfo(type, id, price);
    NVM_PERSIST_NOW(reservation_info, sizeof(ReservationInfo));
    return reservation_info;
}

std::string ToString(Reservation *reservation) {
    std::stringstream ss;
    ss << "id:" << reservation->id 
       << " used:" << reservation->num_used
       << " free:" << reservation->num_free
       << " total:" << reservation->num_total
       << " price:" << reservation->price;
    return ss.str();
}

Reservation* MakeReservation(Reservation *reservation) {
    if (reservation->num_free < 1)
        return nullptr;
    return AllocReservation(reservation->id, reservation->num_used+1,
                            reservation->num_free-1, reservation->num_total,
                            reservation->price);
}

Reservation* CancelReservation(Reservation *reservation) {
    if (reservation->num_used == 0)
        return nullptr;
    return AllocReservation(reservation->id, reservation->num_used-1,
                            reservation->num_free+1, reservation->num_total,
                            reservation->price);
}
