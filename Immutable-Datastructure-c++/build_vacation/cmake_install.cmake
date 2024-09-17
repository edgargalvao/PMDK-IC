# Install script for directory: /nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Immer/ImmerConfig.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Immer/ImmerConfig.cmake"
         "/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/CMakeFiles/Export/lib/cmake/Immer/ImmerConfig.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Immer/ImmerConfig-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Immer/ImmerConfig.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Immer" TYPE FILE FILES "/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/CMakeFiles/Export/lib/cmake/Immer/ImmerConfig.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/immer")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/test/cmake_install.cmake")
  include("/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/benchmark/cmake_install.cmake")
  include("/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/example/cmake_install.cmake")
  include("/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/doc/cmake_install.cmake")
  include("/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/vacation/cmake_install.cmake")
  include("/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/memcached-1.2.4/cmake_install.cmake")
  include("/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/canneal/cmake_install.cmake")
  include("/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/extra/fuzzer/cmake_install.cmake")
  include("/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/extra/python/cmake_install.cmake")
  include("/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/extra/guile/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/nobackup/swapnilh/mod-single-repo/Immutable-Datastructure-c++/build_vacation/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
