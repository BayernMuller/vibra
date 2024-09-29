cmake_minimum_required(VERSION 3.5)
project(vibra)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# enable profiling
option(ENABLE_PROFILING "Enable profiling with -pg flag" OFF)
if(ENABLE_PROFILING)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

add_subdirectory(lib)
add_subdirectory(cli)