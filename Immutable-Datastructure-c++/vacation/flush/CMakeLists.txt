add_executable(vacation ../vacation.cpp ../manager.cpp ../customer.cpp ../reservation.cpp)
target_include_directories(vacation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../  ${VALGRIND_DIR}/include ${VALGRIND_DIR}/lackey)
target_link_libraries(vacation PUBLIC immer-dev)
