
cmake_minimum_required(VERSION 3.5.1)
cmake_policy(SET CMP0048 NEW) # enable project VERSION
cmake_policy(SET CMP0056 NEW) # honor link flags in try_compile()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(immer VERSION 0.4.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-extended-offsetof -ggdb -O3 -mclflushopt -fno-omit-frame-pointer")
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_EXTENSIONS off)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")
endif()

include(ImmerUtils)

#  Options
#  =======

option(ENABLE_SANITIZE "compile with sanitizers enabled")
option(ENABLE_COVERAGE "compile with test coverage support")
option(DISABLE_WERROR "enable --werror")
option(DISABLE_FREE_LIST "disables the free list heap")
option(DISABLE_THREAD_SAFETY "disables thread safety by default")

option(ENABLE_PYTHON "enable building python module" off)
option(ENABLE_GUILE "enable building guile module" off)
option(ENABLE_BOOST_COROUTINE "run benchmarks with boost coroutine" off)
option(ENABLE_NVM "store state in persistent memory" on)

set(CXX_STANDARD 14 CACHE STRING "c++ standard number")

set(CMAKE_CXX_STANDARD ${CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED on)

if (ENABLE_SANITIZE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak")
endif()
if (NOT DISABLE_WERROR)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

#  Dependencies
#  ============

if (ENABLE_BOOST_COROUTINE)
  set(immer_boost_components coroutine)
endif()

find_package(Threads)
find_package(Boost 1.56 COMPONENTS ${immer_boost_components})

find_program(CCACHE ccache)
if (CCACHE)
  message(STATUS "Using ccache: ${CCACHE}")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE})
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE})
else()
  message(STATUS "Could not find ccache")
endif()

#  Targets
#  =======

# the library
add_library(immer INTERFACE)
target_include_directories(immer INTERFACE
  $<BUILD_INTERFACE:${immer_BINARY_DIR}/>
  $<BUILD_INTERFACE:${immer_SOURCE_DIR}/>
  $<INSTALL_INTERFACE:include>)
install(TARGETS immer EXPORT ImmerConfig)
install(EXPORT ImmerConfig DESTINATION lib/cmake/Immer)
install(DIRECTORY immer DESTINATION include)

# development target to be used in tests, examples, benchmarks...
immer_canonicalize_cmake_booleans(
  DISABLE_FREE_LIST
  DISABLE_THREAD_SAFETY
  CHECK_SLOW_TESTS
  ENABLE_NVM)
add_library(immer-dev INTERFACE)
target_include_directories(immer-dev SYSTEM INTERFACE
  ${Boost_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/tools/include
  ${CMAKE_SOURCE_DIR}/../nvm_malloc/src)
target_link_libraries(immer-dev INTERFACE
  immer
  -L${CMAKE_SOURCE_DIR}/../nvm_malloc/
  -lnvmmalloc 
  -lpthread)
target_compile_definitions(immer-dev INTERFACE
  -DIMMER_CXX_STANDARD=${CXX_STANDARD}
  -DIMMER_HAS_LIBGC=1
  -DIMMER_NO_FREE_LIST=${DISABLE_FREE_LIST}
  -DIMMER_NO_THREAD_SAFETY=${DISABLE_THREAD_SAFETY}
  -DIMMER_USE_NVM=${ENABLE_NVM}
  -DIMMER_SLOW_TESTS=${CHECK_SLOW_TESTS})
if (ENABLE_COVERAGE)
  target_compile_options(immer-dev INTERFACE "--coverage")
  target_link_libraries(immer-dev INTERFACE "--coverage")
endif()

#  Testing
#  =======

enable_testing()

add_custom_target(check
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Build and run all the tests and examples.")

add_subdirectory(test)
add_subdirectory(benchmark)
add_subdirectory(example)
add_subdirectory(doc)
add_subdirectory(vacation)
add_subdirectory(memcached-1.2.4)
add_subdirectory(canneal)
add_subdirectory(extra/fuzzer)
add_subdirectory(extra/python)
add_subdirectory(extra/guile)
