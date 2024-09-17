add_executable(memcached_no_flush ../new_assoc.cpp ../items.c ../memcached.c ../thread.c)
target_include_directories(memcached_no_flush PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ ${VALGRIND}/lackey ${VALGRIND}/include)
set_target_properties(memcached_no_flush PROPERTIES COMPILE_FLAGS "-DIMMER_DISABLE_FLUSHING")
target_link_libraries(memcached_no_flush PUBLIC immer-dev event)
