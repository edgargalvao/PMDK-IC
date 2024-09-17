/* =============================================================================
 *
 * vacation.cpp
 *
 * =============================================================================
 *
 * Author: Swapnil Haria 
 * This code is based on the vacation benchmark from the STAMP suite.
 * Similar to vacation in the WHISPER suite, manager object is persistent
 * but clients are not persistent.
 * TODO: make clients persistent?
 * =============================================================================
 */


#include <cassert>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <iostream>
#include <random>
#include <thread>
#include <unistd.h>
#include <sys/stat.h>

#include <lackey.h>
#include <immer/map.hpp>
#include <immer/memory_policy.hpp>
#include <util.hpp>
#include <manager.hpp>
#include <client.hpp>
#include <constants.hpp>
#include <reservation.hpp>
#include <box.hpp>
#include <nvm_malloc.h>

// TODO: Complete memory accounting code.
unsigned long long nv_mem_total = 0;

constexpr auto MAX_CLIENTS= 16;

/*
 * Get the time in milliseconds.
 */
static uint64_t get_time(void)
{
    // Linux:
    struct timespec ts;
    unsigned tick = 0;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    tick  = ts.tv_nsec / 1000000;
    tick += ts.tv_sec * 1000;
    return tick;
}

int prepopulated_relations = kPrepopulatedRelations;
int num_clients = kNumClients;
int num_transactions = kNumTransactions;
int transactions_per_client = kTransactionsPerClient;
int queries_per_transaction = kQueriesPerTransaction;
int percent_query_range = kPercentQueryRange;
int percent_user_queries = kPercentUserQueries; 
int random_seed = 0;

Box* InitializeManager() {
    Box *manager_box = (Box*) nvm_get_id(kManagerId);
    
    if (manager_box != nullptr) {
        const auto* manager_ptr = manager_box->GetManager();
        manager_ptr->AssertInnerTablesNotNull();
        std::cout << "Reusing existing tables\n"; 
        manager_ptr->PrintTables();
        manager_box->ResetMutex();
        return manager_box;
    }
    
    std::cout << "POpulating tables\n";
    
    // Pre-Populate tables with some random values.
    auto* car_table_ptr = new MAP_T();
    auto* room_table_ptr = new MAP_T();
    auto* flight_table_ptr = new MAP_T();
    auto* customer_table_ptr = new CUSTOMER_MAP_T();

    std::mt19937 generator(random_seed);
    std::uniform_int_distribution<int> 
        distribution(1, prepopulated_relations);
    std::uniform_int_distribution<int> 
        price_distribution(1, kMaxPrice);

    // Create list of ids, which get shuffled later.
    std::vector<int> ids (prepopulated_relations);
    for (int i = 0; i < prepopulated_relations; i++) {
        ids[i] = i + 1;
    }

    // We add 1 here because we also add customers.
    for (int table = 0; table < Manager::NumReservationTypes()+1; table++){
        // Randomly shuffle ids.
        std::shuffle(ids.begin(), ids.end(), generator);
        for (int entry = 0; entry < prepopulated_relations; entry++) {
            long id = ids[entry];
            // The math comes straight from original vacation.
            // TODO: not exactly same as original, as our distribution has explicit limits.
            long num = ((distribution(generator) % 5) + 1 ) * 100;
            long price = price_distribution(generator);
            switch (table) {
                case 0: 
                    NOTE("Adding %lu Cars to id:%lu\n", num, id);
                    car_table_ptr = AddReservation(car_table_ptr, id, num, price);
                    break;
                case 1: 
                    NOTE("Adding %lu Flights to id:%lu\n", num, id);
                    flight_table_ptr = AddReservation(flight_table_ptr, id, num, price);
                    break;
                case 2: 
                    NOTE("Adding %lu Rooms to id:%lu\n", num, id);
                    room_table_ptr = AddReservation(room_table_ptr, id, num, price);
                    break;
                case 3:
                    customer_table_ptr = AddCustomer(customer_table_ptr, id);
                    break;
                default : std::cerr << "No such table found." << std::endl ;
            }
        }
    }
    // Ensure all tables are persistent before updating manager.
    NVM_SFENCE();
    auto *manager_ptr = AllocManager(car_table_ptr, room_table_ptr,
           flight_table_ptr, customer_table_ptr);
    NVM_PERSIST_NOW(manager_ptr, sizeof(Manager));
    NVM_SFENCE();
    manager_ptr->PrintTables();

    void *mem = nvm_reserve_id(kManagerId, sizeof(Box));
    manager_box = new (mem) Box(manager_ptr);
    nvm_activate_id(kManagerId);
    return manager_box; 
} 

Manager* RecoverManager() {
    Manager *manager_ptr = (Manager*) nvm_get_id(kManagerId);
    assert(manager_ptr != nullptr);
    manager_ptr->PrintTables();
    return manager_ptr;
}

std::vector<Client*> InitializeClients () {
    std::vector<Client*> clients;
    for (int i = 0; i < num_clients; i++) {
        clients.push_back(
            new Client(i,
                       transactions_per_client,
                       queries_per_transaction,
                       percent_query_range * prepopulated_relations / 100,
                       percent_user_queries));
    }
    return clients;
}

Action SelectAction (long random_percent, long percent_user_queries) {
    if (random_percent < percent_user_queries)
        return ACTION_MAKE_RESERVATION;
    else if (random_percent & 1)
        return ACTION_DELETE_CUSTOMER;
    else 
        return ACTION_UPDATE_TABLES;
}

void ClientRun(Client *client, Box *manager_box) {
    long num_operations = client->num_operations;
    long num_query_per_transaction = client->num_query_per_transaction;
    long query_range = client->query_range;
    long percent_user_queries = client->percent_user_queries;

    // Initialize random generator. Is there a way to factor this out?
    std::mt19937 generator(random_seed);
    std::uniform_int_distribution<int> 
        percent_distribution(0, 99);
    std::uniform_int_distribution<int> 
        query_range_distribution(1, query_range);
    std::uniform_int_distribution<int> 
        num_query_distribution(1, num_query_per_transaction);
    std::uniform_int_distribution<int> 
        reservation_distribution(0, Manager::NumReservationTypes()-1);
    std::uniform_int_distribution<int> 
        binary_distribution(0, 1);
    std::uniform_int_distribution<int> 
        price_distribution(1, kMaxPrice);

    std::vector<long> types (num_query_per_transaction);
    std::vector<long> ids (num_query_per_transaction);
    std::vector<long> ops (num_query_per_transaction);
    std::vector<long> prices (num_query_per_transaction);

    // TODO: Maybe this can be parallelized? Or we can use foreach here?
    for (int i = 0; i < num_operations; i++) {
        long random_percent = percent_distribution(generator);
        auto action = SelectAction(random_percent, percent_user_queries);
        switch (action) {
            case ACTION_MAKE_RESERVATION: {
                // std::cout << "Case: Make Reservation" << std::endl;
                std::vector<long> max_prices (
                    Manager::NumReservationTypes(), -1);
                std::vector<long> max_ids (
                    Manager::NumReservationTypes(), -1);
                long num_queries = num_query_distribution(generator);
                long customer_id = query_range_distribution(generator);
                for (long n = 0; n < num_queries; n++) {
                    types[n] = reservation_distribution(generator);
                    ids[n] = query_range_distribution(generator);
                }
                // bool found = false;
                auto* manager_ptr = manager_box->LockAndReturnManager();
                for (long n = 0; n < num_queries; n++) {
                    long price = -1;
                    switch (types[n]) {
                        case RESERVATION_CAR: {
                            if (manager_ptr->QueryCar(ids[n]) > 0) {
                                price = manager_ptr->QueryCarPrice(ids[n]);
                            }
                            break;
                        }
                        case RESERVATION_FLIGHT: {
                            if (manager_ptr->QueryFlight(ids[n]) > 0) {
                                price = manager_ptr->QueryFlightPrice(ids[n]);
                            }
                            break;
                        }
                        case RESERVATION_ROOM: {
                            if (manager_ptr->QueryRoom(ids[n]) > 0) {
                                price = manager_ptr->QueryRoomPrice(ids[n]);
                            }
                            break;
                        }
                    }
                    // IIUC, we are selling them the most pricy car.
                    if (price > max_prices[types[n]])
                    {
                        max_prices[types[n]] = price;
                        max_ids[types[n]] = ids[n];
                        // found = true;
                    }
                }
                // We can add customer within Reserve* function to 
                // reduce copies.
                /*
                if (found) {
                    manager_ptr = manager_ptr->AddCustomer(customer_id);
                    // std::cout << "Done: ";
                }
                */
                if (max_ids[RESERVATION_CAR] > 0)
                {
                    NOTE("Reserve Car id:%lu for customer:%lu\n", max_ids[RESERVATION_CAR], customer_id);
                    manager_ptr = manager_ptr->ReserveCar(
                        customer_id, max_ids[RESERVATION_CAR]);
                    // std::cout << "Reserved Car ";
                }
                if (max_ids[RESERVATION_FLIGHT] > 0)
                {
                    NOTE("Reserve Flight id:%lu for customer:%lu\n", max_ids[RESERVATION_FLIGHT], customer_id);
                    manager_ptr = manager_ptr->ReserveFlight(
                        customer_id, max_ids[RESERVATION_FLIGHT]);
                    // std::cout << "Reserved Flight";
                }
                if (max_ids[RESERVATION_ROOM] > 0)
                {
                    NOTE("Reserve Room id:%lu for customer:%lu\n", max_ids[RESERVATION_ROOM], customer_id);
                    manager_ptr = manager_ptr->ReserveRoom(
                        customer_id, max_ids[RESERVATION_ROOM]);
                    // std::cout << "Reserved Room";
                }
                manager_box->UpdateManagerAndUnlock(manager_ptr);
                break;
            }
            case ACTION_DELETE_CUSTOMER: {
                long customer_id = query_range_distribution(generator);
                auto* manager_ptr = manager_box->LockAndReturnManager();
                long bill = manager_ptr->QueryCustomerBill(customer_id);
                NOTE("Delete  customer:%lu with bill:%lu\n", customer_id, bill);
                if (bill >= 0)
                    manager_ptr = manager_ptr->DeleteCustomer(customer_id);
                manager_box->UpdateManagerAndUnlock(manager_ptr);
                break;
            }
            case ACTION_UPDATE_TABLES: {
                long num_updates = num_query_distribution(generator);
                for (long n = 0; n < num_updates; n++) {
                    types[n] = reservation_distribution(generator);
                    ids[n] = query_range_distribution(generator);
                    ops[n] = binary_distribution(generator);
                    if (ops[n] == 1) {
                        prices[n] = price_distribution(generator);
                    }
                }

                // TODO: Original uses two for loops, but we can merge?
                auto* manager_ptr = manager_box->LockAndReturnManager();
                for (long n = 0; n < num_updates; n++) {
                    if (ops[n] == 1) {
                        // Add reservations.
                        switch (types[n]) {
                            case RESERVATION_CAR: {
                                // 100 comes from original vacation.
                                NOTE("Adding 100 Cars to id:%lu\n", ids[n]);
                                manager_ptr = manager_ptr->AddCar(ids[n], 100, prices[n]);
                                break;
                            }
                            case RESERVATION_FLIGHT: {
                                // 100 comes from original vacation.
                                NOTE("Adding 100 Flights to id:%lu\n", ids[n]);
                                manager_ptr = manager_ptr->AddFlight(ids[n], 100, prices[n]);
                                break;
                            }
                            case RESERVATION_ROOM: {
                                // 100 comes from original vacation.
                                NOTE("Adding 100 Rooms to id:%lu\n", ids[n]);
                                manager_ptr = manager_ptr->AddRoom(ids[n], 100, prices[n]);
                                break;
                            }
                            default:
                            {
                                std::cerr << "Reservation type not supported:"
                                          << types[n] << std::endl;
                            }
                        }
                    }
                    else
                    {
                        // Delete reservations.
                         switch (types[n]) {
                            case RESERVATION_CAR: {
                                NOTE("Deleting 100 Cars from id:%lu\n", ids[n]);
                                manager_ptr = manager_ptr->DeleteCar(ids[n], 100);
                                break;
                            }
                            case RESERVATION_FLIGHT: {
                                NOTE("Deleting 100 Flights from id:%lu\n", ids[n]);
                                manager_ptr = manager_ptr->DeleteFlight(ids[n]);
                                break;
                            }
                            case RESERVATION_ROOM: {
                                NOTE("Deleting 100 Rooms from id:%lu\n", ids[n]);
                                manager_ptr = manager_ptr->DeleteRoom(ids[n], 100);
                                break;
                            }
                            default:
                            {
                                std::cerr << "Reservation type not supported:"
                                          << types[n] << std::endl;
                            }
                        }
                   }
                }
                manager_box->UpdateManagerAndUnlock(manager_ptr);
                break;
            }
            default:
            {
                std::cerr << "Incorrect action type selected:" << random_percent;
            }
        }
        NVM_SFENCE();
    }
}

bool BackingFileExists(const char* path) {
    char backing_file[300];
    strcpy(backing_file, path);
    strcat(backing_file, "/backing");
    struct stat buffer;
    return (stat (backing_file, &buffer) == 0);
}

void BeginValgrindInstrumentation() {
    // Stop and Start flushes all counters
    LACKEY_STOP_INSTRUMENTATION;
    LACKEY_START_INSTRUMENTATION;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "Error! Correct Usage: ./vacation backing-file <vacation options>\n";
        return 0;
    }
    int opt;
    const char* path = argv[1];
    while ((opt = getopt(argc, argv, "c:n:q:t:r:u:p:s:")) != -1) {
        switch (opt) {
            case 'c':
                num_clients = atoi(optarg);
                assert(num_clients <= MAX_CLIENTS);
                break;
            case 'n':
                queries_per_transaction = atoi(optarg);
                break;
            case 'q':
                percent_query_range = atoi(optarg);
                break;
            case 't':
                num_transactions = atoi(optarg);
                break;
            case 'r':
                prepopulated_relations = atoi(optarg);
                break;
            case 'u':
                percent_user_queries = atoi(optarg);
                break;
            case 'p':
                path = optarg;
                break;
            case 's':
                random_seed = atoi(optarg);
                break;
        }
    }
    if (random_seed == 0) {
        std::random_device device;
        random_seed = device();
    }

    transactions_per_client = num_transactions/num_clients;
    Box *manager_box;
    bool recover = BackingFileExists(path);
    if (recover == false) {
        std::cout << "Initializing backing file\n"; 
        nvm_initialize(path, 0);
        size_t t0 = get_time();
        manager_box = InitializeManager();
        size_t t1 = get_time();
        std::cout << "Init time:" << (t1 - t0) << std::endl;
        assert(manager_box);
        
    } else {
        // recovered
        nvm_initialize(path, 1);
        std::cout << "Recovering from backing file\n"; 
        manager_box = InitializeManager();
        assert(manager_box);
    }
    std::thread client_threads[MAX_CLIENTS];
    auto clients = InitializeClients();

    BeginValgrindInstrumentation();
    // TODO: Maybe use std::for_each here to get across the point with functional code?
    size_t t0 = get_time();
    for (unsigned int i = 0; i < clients.size(); i++)
    {
        client_threads[i] = std::thread(ClientRun, clients[i], manager_box);
    }
    for (unsigned int i = 0; i < clients.size(); i++)
    {
        client_threads[i].join();
    }
    size_t t1 = get_time();
    std::cout << "Run time:" << (t1 - t0) << std::endl;
    
    const auto* const_manager_ptr = manager_box->GetManager();
    const_manager_ptr->PrintTables();
    return 0;
}
