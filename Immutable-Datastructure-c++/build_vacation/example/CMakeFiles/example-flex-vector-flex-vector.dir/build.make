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
include example/CMakeFiles/example-flex-vector-flex-vector.dir/depend.make

# Include the progress variables for this target.
include example/CMakeFiles/example-flex-vector-flex-vector.dir/progress.make

# Include the compile flags for this target's objects.
include example/CMakeFiles/example-flex-vector-flex-vector.dir/flags.make

example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o: example/CMakeFiles/example-flex-vector-flex-vector.dir/flags.make
example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o: ../example/flex-vector/flex-vector.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o -c /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/example/flex-vector/flex-vector.cpp

example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.i"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/example/flex-vector/flex-vector.cpp > CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.i

example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.s"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/example/flex-vector/flex-vector.cpp -o CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.s

example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o.requires:

.PHONY : example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o.requires

example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o.provides: example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o.requires
	$(MAKE) -f example/CMakeFiles/example-flex-vector-flex-vector.dir/build.make example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o.provides.build
.PHONY : example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o.provides

example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o.provides.build: example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o


# Object files for target example-flex-vector-flex-vector
example__flex__vector__flex__vector_OBJECTS = \
"CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o"

# External object files for target example-flex-vector-flex-vector
example__flex__vector__flex__vector_EXTERNAL_OBJECTS =

example/flex-vector-flex-vector: example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o
example/flex-vector-flex-vector: example/CMakeFiles/example-flex-vector-flex-vector.dir/build.make
example/flex-vector-flex-vector: example/CMakeFiles/example-flex-vector-flex-vector.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable flex-vector-flex-vector"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/example-flex-vector-flex-vector.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
example/CMakeFiles/example-flex-vector-flex-vector.dir/build: example/flex-vector-flex-vector

.PHONY : example/CMakeFiles/example-flex-vector-flex-vector.dir/build

example/CMakeFiles/example-flex-vector-flex-vector.dir/requires: example/CMakeFiles/example-flex-vector-flex-vector.dir/flex-vector/flex-vector.cpp.o.requires

.PHONY : example/CMakeFiles/example-flex-vector-flex-vector.dir/requires

example/CMakeFiles/example-flex-vector-flex-vector.dir/clean:
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example && $(CMAKE_COMMAND) -P CMakeFiles/example-flex-vector-flex-vector.dir/cmake_clean.cmake
.PHONY : example/CMakeFiles/example-flex-vector-flex-vector.dir/clean

example/CMakeFiles/example-flex-vector-flex-vector.dir/depend:
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++ /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/example /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example/CMakeFiles/example-flex-vector-flex-vector.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : example/CMakeFiles/example-flex-vector-flex-vector.dir/depend

