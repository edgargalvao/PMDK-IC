#pragma once

#include <string>
#include <immer/memory_policy.hpp>

struct Reservation {
    
    void* operator new(size_t size) {
        return immer::default_memory_policy::heap::type::allocate(size);
    }

    void operator delete(void* data) {
        immer::default_memory_policy::heap::type::deallocate(0, data);
    }

    void operator delete(void* data, size_t size) {
        immer::default_memory_policy::heap::type::deallocate(size, data);
    }

    Reservation(long id_in, long num_used_in, long num_free_in,
                long num_total_in, long price_in)
        : id(id_in),
          num_used(num_used_in),
          num_free(num_free_in),
          num_total(num_total_in),
          price(price_in)
    {}

    // TODO: id is in key as well as value.
    long id;
    long num_used;
    long num_free;
    // TODO: numTotal is just numUsed + numFree.
    long num_total;
    long price;
};

// We do not use enum class here as we generate a random long
// to select between these types.
enum ReservationType {
  RESERVATION_CAR = 0,
  RESERVATION_FLIGHT = 1,
  RESERVATION_ROOM = 2,
  NUM_RESERVATION_TYPES
};

struct ReservationInfo {
    ReservationInfo (ReservationType type_in, long id_in, long price_in)
        : type (type_in), id (id_in), price (price_in) {}

    ReservationType type;
    long id;
    long price; /* holds price at time reservation was made */
};


Reservation* AllocReservation(long id, long num_used, long num_free,
                              long num_total, long price);

// Here id is car/flight/room id depending on type
ReservationInfo* AllocReservationInfo(ReservationType type, long id,
                                      long price);

std::string ToString(Reservation *reservation);

Reservation* MakeReservation(Reservation * reservation);

Reservation* CancelReservation(Reservation * reservation);
