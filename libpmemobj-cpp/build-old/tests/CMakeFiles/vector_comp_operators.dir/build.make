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
include tests/CMakeFiles/vector_comp_operators.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/vector_comp_operators.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/vector_comp_operators.dir/flags.make

tests/CMakeFiles/vector_comp_operators.dir/vector_comp_operators/vector_comp_operators.cpp.o: tests/CMakeFiles/vector_comp_operators.dir/flags.make
tests/CMakeFiles/vector_comp_operators.dir/vector_comp_operators/vector_comp_operators.cpp.o: ../tests/vector_comp_operators/vector_comp_operators.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/CMakeFiles/vector_comp_operators.dir/vector_comp_operators/vector_comp_operators.cpp.o"
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/vector_comp_operators.dir/vector_comp_operators/vector_comp_operators.cpp.o -c /home/swapnilh/pmdk-stuff/libpmemobj-cpp/tests/vector_comp_operators/vector_comp_operators.cpp

tests/CMakeFiles/vector_comp_operators.dir/vector_comp_operators/vector_comp_operators.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vector_comp_operators.dir/vector_comp_operators/vector_comp_operators.cpp.i"
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/swapnilh/pmdk-stuff/libpmemobj-cpp/tests/vector_comp_operators/vector_comp_operators.cpp > CMakeFiles/vector_comp_operators.dir/vector_comp_operators/vector_comp_operators.cpp.i

tests/CMakeFiles/vector_comp_operators.dir/vector_comp_operators/vector_comp_operators.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vector_comp_operators.dir/vector_comp_operators/vector_comp_operators.cpp.s"
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/swapnilh/pmdk-stuff/libpmemobj-cpp/tests/vector_comp_operators/vector_comp_operators.cpp -o CMakeFiles/vector_comp_operators.dir/vector_comp_operators/vector_comp_operators.cpp.s

# Object files for target vector_comp_operators
vector_comp_operators_OBJECTS = \
"CMakeFiles/vector_comp_operators.dir/vector_comp_operators/vector_comp_operators.cpp.o"

# External object files for target vector_comp_operators
vector_comp_operators_EXTERNAL_OBJECTS =

tests/vector_comp_operators: tests/CMakeFiles/vector_comp_operators.dir/vector_comp_operators/vector_comp_operators.cpp.o
tests/vector_comp_operators: tests/CMakeFiles/vector_comp_operators.dir/build.make
tests/vector_comp_operators: tests/libtest_backtrace.a
tests/vector_comp_operators: tests/libvalgrind_internal.a
tests/vector_comp_operators: tests/CMakeFiles/vector_comp_operators.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable vector_comp_operators"
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vector_comp_operators.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/vector_comp_operators.dir/build: tests/vector_comp_operators

.PHONY : tests/CMakeFiles/vector_comp_operators.dir/build

tests/CMakeFiles/vector_comp_operators.dir/clean:
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests && $(CMAKE_COMMAND) -P CMakeFiles/vector_comp_operators.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/vector_comp_operators.dir/clean

tests/CMakeFiles/vector_comp_operators.dir/depend:
	cd /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/swapnilh/pmdk-stuff/libpmemobj-cpp /home/swapnilh/pmdk-stuff/libpmemobj-cpp/tests /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests /home/swapnilh/pmdk-stuff/libpmemobj-cpp/build/tests/CMakeFiles/vector_comp_operators.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/vector_comp_operators.dir/depend

