cmake_minimum_required(VERSION 3.14)
project(vibra)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    # TODO: fix warnings
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -pedantic -fno-strict-aliasing")
endif()
include_directories(${CMAKE_SOURCE_DIR}/lib)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /Zi")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
    endif()
else()
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    endif()
endif()

# build verbose output
# set(CMAKE_VERBOSE_MAKEFILE ON)

# enable profiling
option(ENABLE_PROFILING "Enable profiling with -pg flag" OFF)
if(ENABLE_PROFILING)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

option(LIBRARY_ONLY "Build only the library" OFF)
option(BUILD_JS "Build JavaScript/WebAssembly bindings" OFF)
include(CTest)

add_subdirectory(lib)

if(BUILD_JS)
    add_subdirectory(js)
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(NOT LIBRARY_ONLY)
    add_subdirectory(cli)
endif()
