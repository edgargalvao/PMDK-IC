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

# Include any dependencies generated for this target.
include test/CMakeFiles/test-memory-refcounts.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/test-memory-refcounts.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/test-memory-refcounts.dir/flags.make

test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o: test/CMakeFiles/test-memory-refcounts.dir/flags.make
test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o: ../test/memory/refcounts.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o -c /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/test/memory/refcounts.cpp

test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.i"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/test/memory/refcounts.cpp > CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.i

test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.s"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/test/memory/refcounts.cpp -o CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.s

test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o.requires:

.PHONY : test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o.requires

test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o.provides: test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o.requires
	$(MAKE) -f test/CMakeFiles/test-memory-refcounts.dir/build.make test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o.provides.build
.PHONY : test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o.provides

test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o.provides.build: test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o


# Object files for target test-memory-refcounts
test__memory__refcounts_OBJECTS = \
"CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o"

# External object files for target test-memory-refcounts
test__memory__refcounts_EXTERNAL_OBJECTS =

test/memory-refcounts: test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o
test/memory-refcounts: test/CMakeFiles/test-memory-refcounts.dir/build.make
test/memory-refcounts: test/CMakeFiles/test-memory-refcounts.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable memory-refcounts"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-memory-refcounts.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/test-memory-refcounts.dir/build: test/memory-refcounts

.PHONY : test/CMakeFiles/test-memory-refcounts.dir/build

test/CMakeFiles/test-memory-refcounts.dir/requires: test/CMakeFiles/test-memory-refcounts.dir/memory/refcounts.cpp.o.requires

.PHONY : test/CMakeFiles/test-memory-refcounts.dir/requires

test/CMakeFiles/test-memory-refcounts.dir/clean:
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test && $(CMAKE_COMMAND) -P CMakeFiles/test-memory-refcounts.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/test-memory-refcounts.dir/clean

test/CMakeFiles/test-memory-refcounts.dir/depend:
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++ /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/test /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test/CMakeFiles/test-memory-refcounts.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/test-memory-refcounts.dir/depend

