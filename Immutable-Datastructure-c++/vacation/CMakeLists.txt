add_custom_target(immer_vacation COMMENT "Builds vacation")
set(VALGRIND_DIR "../../../pmem-valgrind/")
add_subdirectory("flush")
add_subdirectory("no_flush")
