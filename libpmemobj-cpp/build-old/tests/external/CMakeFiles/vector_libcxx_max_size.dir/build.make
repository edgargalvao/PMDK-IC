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
include tests/external/CMakeFiles/vector_libcxx_max_size.dir/depend.make

# Include the progress variables for this target.
include tests/external/CMakeFiles/vector_libcxx_max_size.dir/progress.make

# Include the compile flags for this target's objects.
include tests/external/CMakeFiles/vector_libcxx_max_size.dir/flags.make

tests/external/CMakeFiles/vector_libcxx_max_size.dir/libcxx/vector/vector.capacity/max_size.pass.cpp.o: tests/external/CMakeFiles/vector_libcxx_max_size.dir/flags.make
tests/external/CMakeFiles/vector_libcxx_max_size.dir/libcxx/vector/vector.capacity/max_size.pass.cpp.o: ../tests/external/libcxx/vector/vector.capacity/max_size.pass.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/external/CMakeFiles/vector_libcxx_max_size.dir/libcxx/vector/vector.capacity/max_size.pass.cpp.o"
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/external && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/vector_libcxx_max_size.dir/libcxx/vector/vector.capacity/max_size.pass.cpp.o -c /home/swapnilh/pmdk-stuff/libpmemobj-cpp/tests/external/libcxx/vector/vector.capacity/max_size.pass.cpp

tests/external/CMakeFiles/vector_libcxx_max_size.dir/libcxx/vector/vector.capacity/max_size.pass.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vector_libcxx_max_size.dir/libcxx/vector/vector.capacity/max_size.pass.cpp.i"
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/external && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/swapnilh/pmdk-stuff/libpmemobj-cpp/tests/external/libcxx/vector/vector.capacity/max_size.pass.cpp > CMakeFiles/vector_libcxx_max_size.dir/libcxx/vector/vector.capacity/max_size.pass.cpp.i

tests/external/CMakeFiles/vector_libcxx_max_size.dir/libcxx/vector/vector.capacity/max_size.pass.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vector_libcxx_max_size.dir/libcxx/vector/vector.capacity/max_size.pass.cpp.s"
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/external && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/swapnilh/pmdk-stuff/libpmemobj-cpp/tests/external/libcxx/vector/vector.capacity/max_size.pass.cpp -o CMakeFiles/vector_libcxx_max_size.dir/libcxx/vector/vector.capacity/max_size.pass.cpp.s

# Object files for target vector_libcxx_max_size
vector_libcxx_max_size_OBJECTS = \
"CMakeFiles/vector_libcxx_max_size.dir/libcxx/vector/vector.capacity/max_size.pass.cpp.o"

# External object files for target vector_libcxx_max_size
vector_libcxx_max_size_EXTERNAL_OBJECTS =

tests/external/vector_libcxx_max_size: tests/external/CMakeFiles/vector_libcxx_max_size.dir/libcxx/vector/vector.capacity/max_size.pass.cpp.o
tests/external/vector_libcxx_max_size: tests/external/CMakeFiles/vector_libcxx_max_size.dir/build.make
tests/external/vector_libcxx_max_size: tests/libtest_backtrace.a
tests/external/vector_libcxx_max_size: tests/libvalgrind_internal.a
tests/external/vector_libcxx_max_size: tests/external/CMakeFiles/vector_libcxx_max_size.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable vector_libcxx_max_size"
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/external && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vector_libcxx_max_size.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/external/CMakeFiles/vector_libcxx_max_size.dir/build: tests/external/vector_libcxx_max_size

.PHONY : tests/external/CMakeFiles/vector_libcxx_max_size.dir/build

tests/external/CMakeFiles/vector_libcxx_max_size.dir/clean:
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/external && $(CMAKE_COMMAND) -P CMakeFiles/vector_libcxx_max_size.dir/cmake_clean.cmake
.PHONY : tests/external/CMakeFiles/vector_libcxx_max_size.dir/clean

tests/external/CMakeFiles/vector_libcxx_max_size.dir/depend:
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/swapnilh/pmdk-stuff/libpmemobj-cpp /home/swapnilh/pmdk-stuff/libpmemobj-cpp/tests/external /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/external /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/external/CMakeFiles/vector_libcxx_max_size.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/external/CMakeFiles/vector_libcxx_max_size.dir/depend

