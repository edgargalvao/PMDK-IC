# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.11

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/swapnilh/pmdk-stuff/libpmemobj-cpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build

# Include any dependencies generated for this target.
include tests/external/CMakeFiles/array_get_fail_expected.dir/depend.make

# Include the progress variables for this target.
include tests/external/CMakeFiles/array_get_fail_expected.dir/progress.make

# Include the compile flags for this target's objects.
include tests/external/CMakeFiles/array_get_fail_expected.dir/flags.make

tests/external/CMakeFiles/array_get_fail_expected.dir/libcxx/array/array.tuple/get.fail.cpp.o: tests/external/CMakeFiles/array_get_fail_expected.dir/flags.make
tests/external/CMakeFiles/array_get_fail_expected.dir/libcxx/array/array.tuple/get.fail.cpp.o: ../tests/external/libcxx/array/array.tuple/get.fail.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/external/CMakeFiles/array_get_fail_expected.dir/libcxx/array/array.tuple/get.fail.cpp.o"
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/external && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/array_get_fail_expected.dir/libcxx/array/array.tuple/get.fail.cpp.o -c /home/swapnilh/pmdk-stuff/libpmemobj-cpp/tests/external/libcxx/array/array.tuple/get.fail.cpp

tests/external/CMakeFiles/array_get_fail_expected.dir/libcxx/array/array.tuple/get.fail.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/array_get_fail_expected.dir/libcxx/array/array.tuple/get.fail.cpp.i"
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/external && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/swapnilh/pmdk-stuff/libpmemobj-cpp/tests/external/libcxx/array/array.tuple/get.fail.cpp > CMakeFiles/array_get_fail_expected.dir/libcxx/array/array.tuple/get.fail.cpp.i

tests/external/CMakeFiles/array_get_fail_expected.dir/libcxx/array/array.tuple/get.fail.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/array_get_fail_expected.dir/libcxx/array/array.tuple/get.fail.cpp.s"
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/external && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/swapnilh/pmdk-stuff/libpmemobj-cpp/tests/external/libcxx/array/array.tuple/get.fail.cpp -o CMakeFiles/array_get_fail_expected.dir/libcxx/array/array.tuple/get.fail.cpp.s

# Object files for target array_get_fail_expected
array_get_fail_expected_OBJECTS = \
"CMakeFiles/array_get_fail_expected.dir/libcxx/array/array.tuple/get.fail.cpp.o"

# External object files for target array_get_fail_expected
array_get_fail_expected_EXTERNAL_OBJECTS =

tests/external/array_get_fail_expected: tests/external/CMakeFiles/array_get_fail_expected.dir/libcxx/array/array.tuple/get.fail.cpp.o
tests/external/array_get_fail_expected: tests/external/CMakeFiles/array_get_fail_expected.dir/build.make
tests/external/array_get_fail_expected: tests/external/CMakeFiles/array_get_fail_expected.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable array_get_fail_expected"
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/external && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/array_get_fail_expected.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/external/CMakeFiles/array_get_fail_expected.dir/build: tests/external/array_get_fail_expected

.PHONY : tests/external/CMakeFiles/array_get_fail_expected.dir/build

tests/external/CMakeFiles/array_get_fail_expected.dir/clean:
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/external && $(CMAKE_COMMAND) -P CMakeFiles/array_get_fail_expected.dir/cmake_clean.cmake
.PHONY : tests/external/CMakeFiles/array_get_fail_expected.dir/clean

tests/external/CMakeFiles/array_get_fail_expected.dir/depend:
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/swapnilh/pmdk-stuff/libpmemobj-cpp /home/swapnilh/pmdk-stuff/libpmemobj-cpp/tests/external /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/external /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/external/CMakeFiles/array_get_fail_expected.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/external/CMakeFiles/array_get_fail_expected.dir/depend

