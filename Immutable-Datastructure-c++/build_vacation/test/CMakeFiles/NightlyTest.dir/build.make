# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation

# Utility rule file for NightlyTest.

# Include the progress variables for this target.
include test/CMakeFiles/NightlyTest.dir/progress.make

test/CMakeFiles/NightlyTest:
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test && /usr/bin/ctest -D NightlyTest

NightlyTest: test/CMakeFiles/NightlyTest
NightlyTest: test/CMakeFiles/NightlyTest.dir/build.make

.PHONY : NightlyTest

# Rule to build all files generated by this target.
test/CMakeFiles/NightlyTest.dir/build: NightlyTest

.PHONY : test/CMakeFiles/NightlyTest.dir/build

test/CMakeFiles/NightlyTest.dir/clean:
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test && $(CMAKE_COMMAND) -P CMakeFiles/NightlyTest.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/NightlyTest.dir/clean

test/CMakeFiles/NightlyTest.dir/depend:
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++ /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/test /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test/CMakeFiles/NightlyTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/NightlyTest.dir/depend

