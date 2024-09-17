add_custom_target(stamp-kozy COMMENT "Builds vacation")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAP_USE_RBTREE -DLIST_NO_DUPLICATES")
add_dependencies(check stamp-kozy)
# get all *.cpp files recursively
add_executable(vacation customer.c client.c reservation.c manager.c vacation.c list.c pair.c mt19937ar.c random.c rbtree.c thread.c)
target_include_directories(vacation PUBLIC ${CMAKE_SOURCE_DIR}/lib)
