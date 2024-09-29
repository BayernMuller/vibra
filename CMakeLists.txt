cmake_minimum_required(VERSION 3.5)
project(vibra)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall  -Wextra -pedantic -O2")

# build verbose output
set(CMAKE_VERBOSE_MAKEFILE ON)

# enable profiling
option(ENABLE_PROFILING "Enable profiling with -pg flag" OFF)
if(ENABLE_PROFILING)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

add_subdirectory(lib)
add_subdirectory(cli)