cmake_minimum_required(VERSION 3.5)
project(vibra)

# Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -O2")

# Enable verbose output
set(CMAKE_VERBOSE_MAKEFILE ON)

# Check if an external include path is provided and use it if present
if(DEFINED INCLUDE_PATH)
    include_directories(${INCLUDE_PATH})
endif()

# Check if an external library path is provided and use it if present
if(DEFINED LIBRARY_PATH)
    link_directories(${LIBRARY_PATH})
endif()

add_executable(vibra 
    cli/main.cpp
    cli/cli.cpp
    fingerprinting/algorithm/signature.cpp
    fingerprinting/algorithm/frequency.cpp
    fingerprinting/algorithm/signature_generator.cpp
    fingerprinting/audio/wav.cpp
    communication/shazam.cpp
)

# Find required libraries
find_library(FFTW3_LIBRARY NAMES fftw3)
find_library(CURL_LIBRARY NAMES curl)

# Link libraries
target_link_libraries(vibra PRIVATE ${FFTW3_LIBRARY} ${CURL_LIBRARY})
