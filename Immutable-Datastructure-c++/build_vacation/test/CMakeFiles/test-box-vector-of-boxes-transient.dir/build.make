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
include test/CMakeFiles/test-box-vector-of-boxes-transient.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/test-box-vector-of-boxes-transient.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/test-box-vector-of-boxes-transient.dir/flags.make

test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o: test/CMakeFiles/test-box-vector-of-boxes-transient.dir/flags.make
test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o: ../test/box/vector-of-boxes-transient.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o -c /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/test/box/vector-of-boxes-transient.cpp

test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.i"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/test/box/vector-of-boxes-transient.cpp > CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.i

test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.s"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/test/box/vector-of-boxes-transient.cpp -o CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.s

test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o.requires:

.PHONY : test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o.requires

test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o.provides: test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o.requires
	$(MAKE) -f test/CMakeFiles/test-box-vector-of-boxes-transient.dir/build.make test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o.provides.build
.PHONY : test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o.provides

test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o.provides.build: test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o


# Object files for target test-box-vector-of-boxes-transient
test__box__vector__of__boxes__transient_OBJECTS = \
"CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o"

# External object files for target test-box-vector-of-boxes-transient
test__box__vector__of__boxes__transient_EXTERNAL_OBJECTS =

test/box-vector-of-boxes-transient: test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o
test/box-vector-of-boxes-transient: test/CMakeFiles/test-box-vector-of-boxes-transient.dir/build.make
test/box-vector-of-boxes-transient: test/CMakeFiles/test-box-vector-of-boxes-transient.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable box-vector-of-boxes-transient"
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-box-vector-of-boxes-transient.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/test-box-vector-of-boxes-transient.dir/build: test/box-vector-of-boxes-transient

.PHONY : test/CMakeFiles/test-box-vector-of-boxes-transient.dir/build

test/CMakeFiles/test-box-vector-of-boxes-transient.dir/requires: test/CMakeFiles/test-box-vector-of-boxes-transient.dir/box/vector-of-boxes-transient.cpp.o.requires

.PHONY : test/CMakeFiles/test-box-vector-of-boxes-transient.dir/requires

test/CMakeFiles/test-box-vector-of-boxes-transient.dir/clean:
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test && $(CMAKE_COMMAND) -P CMakeFiles/test-box-vector-of-boxes-transient.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/test-box-vector-of-boxes-transient.dir/clean

test/CMakeFiles/test-box-vector-of-boxes-transient.dir/depend:
	cd /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++ /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/test /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test/CMakeFiles/test-box-vector-of-boxes-transient.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/test-box-vector-of-boxes-transient.dir/depend

