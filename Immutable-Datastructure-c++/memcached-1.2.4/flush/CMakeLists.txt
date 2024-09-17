add_executable(memcached ../new_assoc.cpp ../items.c ../memcached.c ../thread.c)
target_include_directories(memcached PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ ${VALGRIND}/lackey ${VALGRIND}/include)
target_link_libraries(memcached PUBLIC immer-dev event)
