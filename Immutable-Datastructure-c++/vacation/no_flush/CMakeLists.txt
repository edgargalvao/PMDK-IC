add_executable(vacation_no_flush ../vacation.cpp ../manager.cpp ../customer.cpp ../reservation.cpp)
set_target_properties(vacation_no_flush PROPERTIES COMPILE_FLAGS "-DIMMER_DISABLE_FLUSHING")
target_include_directories(vacation_no_flush PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/../ ${VALGRIND_DIR}/include ${VALGRIND_DIR}/lackey)
target_link_libraries(vacation_no_flush PUBLIC immer-dev)
