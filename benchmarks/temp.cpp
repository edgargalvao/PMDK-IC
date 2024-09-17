#include <iostream>

#define IMMER_USE_NVM true
#include <immer/detail/list/list.hpp>

using namespace std;


int main (int argc, char** argv) {
    nvm_initialize(argv[1], 0);
    immer::list<uint64_t> listOne({1, 2, 3, 4, 5, 6, 7});
    auto listTwo = listOne.insertedAt(3, 6);
    cout << "listTwo head located at:" << (void*) &(listTwo._head) << endl;
    return 0;
}
