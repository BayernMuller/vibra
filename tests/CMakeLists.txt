cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

find_library(FFTW3_LIBRARY NAMES fftw3 REQUIRED)
find_library(GTEST_LIBRARY NAMES gtest REQUIRED)
find_library(GTEST_MAIN_LIBRARY NAMES gtest_main REQUIRED)

add_executable(all_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/unit_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/algorithm/frequency.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/algorithm/signature.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/algorithm/signature_generator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/audio/wav.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/audio/downsampler.cpp
)

target_include_directories(all_tests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include 
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib
)

target_link_libraries(all_tests PRIVATE ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} ${FFTW3_LIBRARY})

# For google test, C++17 is required
set_target_properties(all_tests PROPERTIES CXX_STANDARD 17)

include(GoogleTest)
gtest_discover_tests(all_tests)