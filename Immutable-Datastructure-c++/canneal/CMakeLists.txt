add_custom_target(parsec_canneal COMMENT "Builds canneal")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_THREADS -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(VALGRIND "/home/swapnilh/valgrind/pmem-valgrind/")
add_dependencies(check canneal)
add_executable(canneal annealer_thread.cpp rng.cpp netlist.cpp main.cpp netlist_elem.cpp)
target_include_directories(canneal PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${VALGRIND}/lackey ${VALGRIND}/include)
target_link_libraries(canneal PUBLIC immer-dev)
