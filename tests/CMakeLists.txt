cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

find_library(FFTW3_LIBRARY NAMES fftw3)
find_library(GTEST_LIBRARY NAMES gtest)
find_library(GTEST_MAIN_LIBRARY NAMES gtest_main)

# 테스트 바이너리 생성
add_executable(all_tests
    lib_test.cpp
    ../lib/algorithm/frequency.cpp
)

# include 디렉토리 추가
target_include_directories(all_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/../include 
    ${CMAKE_SOURCE_DIR}/../lib
)

# 타겟 링크
target_link_libraries(all_tests PRIVATE ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} ${FFTW3_LIBRARY})

# 테스트 자동 발견 및 등록
include(GoogleTest)
gtest_discover_tests(all_tests)