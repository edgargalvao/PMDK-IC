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
include example/CMakeFiles/example-vector-iota-move.dir/depend.make

# Include the progress variables for this target.
include example/CMakeFiles/example-vector-iota-move.dir/progress.make

# Include the compile flags for this target's objects.
include example/CMakeFiles/example-vector-iota-move.dir/flags.make

example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o: example/CMakeFiles/example-vector-iota-move.dir/flags.make
example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o: ../example/vector/iota-move.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o -c /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/example/vector/iota-move.cpp

example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.i"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/example/vector/iota-move.cpp > CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.i

example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.s"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/example/vector/iota-move.cpp -o CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.s

example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o.requires:

.PHONY : example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o.requires

example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o.provides: example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o.requires
	$(MAKE) -f example/CMakeFiles/example-vector-iota-move.dir/build.make example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o.provides.build
.PHONY : example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o.provides

example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o.provides.build: example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o


# Object files for target example-vector-iota-move
example__vector__iota__move_OBJECTS = \
"CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o"

# External object files for target example-vector-iota-move
example__vector__iota__move_EXTERNAL_OBJECTS =

example/vector-iota-move: example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o
example/vector-iota-move: example/CMakeFiles/example-vector-iota-move.dir/build.make
example/vector-iota-move: example/CMakeFiles/example-vector-iota-move.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable vector-iota-move"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/example-vector-iota-move.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
example/CMakeFiles/example-vector-iota-move.dir/build: example/vector-iota-move

.PHONY : example/CMakeFiles/example-vector-iota-move.dir/build

example/CMakeFiles/example-vector-iota-move.dir/requires: example/CMakeFiles/example-vector-iota-move.dir/vector/iota-move.cpp.o.requires

.PHONY : example/CMakeFiles/example-vector-iota-move.dir/requires

example/CMakeFiles/example-vector-iota-move.dir/clean:
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example && $(CMAKE_COMMAND) -P CMakeFiles/example-vector-iota-move.dir/cmake_clean.cmake
.PHONY : example/CMakeFiles/example-vector-iota-move.dir/clean

example/CMakeFiles/example-vector-iota-move.dir/depend:
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++ /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/example /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example/CMakeFiles/example-vector-iota-move.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : example/CMakeFiles/example-vector-iota-move.dir/depend

