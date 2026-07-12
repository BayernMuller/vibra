include(FetchContent)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

add_executable(vibra_unit_tests
    algorithm/frequency_test.cpp
    algorithm/signature_generator_test.cpp
    algorithm/signature_test.cpp
    audio/byte_control_test.cpp
    audio/downsampler_test.cpp
    audio/wav_test.cpp
    public_api_test.cpp
    utils/base64_test.cpp
    utils/crc32_test.cpp
    utils/fft_test.cpp
    utils/ring_buffer_test.cpp
)

target_include_directories(vibra_unit_tests PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/lib
    ${PROJECT_SOURCE_DIR}/third-party/kissfft
)
target_compile_definitions(vibra_unit_tests PRIVATE kiss_fft_scalar=double)
target_link_libraries(vibra_unit_tests PRIVATE
    vibra_static
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(vibra_unit_tests)
