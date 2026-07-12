# Define the sources for the library
set(LIBVIBRA_SOURCES
    vibra.cpp
    algorithm/signature.cpp
    algorithm/frequency.cpp
    algorithm/signature_generator.cpp
    audio/wav.cpp
    audio/downsampler.cpp
    ${CMAKE_SOURCE_DIR}/third-party/kissfft/kiss_fft.c
    ${CMAKE_SOURCE_DIR}/third-party/kissfft/kiss_fftr.c
)

# Add shared and static libraries for libvibra
add_library(vibra_shared SHARED ${LIBVIBRA_SOURCES})
add_library(vibra_static STATIC ${LIBVIBRA_SOURCES})

target_include_directories(vibra_shared PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(vibra_static PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(vibra_shared PRIVATE ${CMAKE_SOURCE_DIR}/third-party/kissfft)
target_include_directories(vibra_static PRIVATE ${CMAKE_SOURCE_DIR}/third-party/kissfft)
target_compile_definitions(vibra_shared PRIVATE kiss_fft_scalar=double)
target_compile_definitions(vibra_static PRIVATE kiss_fft_scalar=double)

if(NOT MSVC)
    target_link_libraries(vibra_shared PRIVATE m)
    target_link_libraries(vibra_static PRIVATE m)
endif()

# Set C++11 standard
set_target_properties(vibra_shared PROPERTIES CXX_STANDARD 11)
set_target_properties(vibra_static PROPERTIES CXX_STANDARD 11)

# Set output names
if (DEFINED BUILD_SUFFIX)
    message(STATUS "Adding suffix: ${BUILD_SUFFIX}")
    set(SUFFIX "-${BUILD_SUFFIX}")
else()
    set(SUFFIX "")
endif()
set_target_properties(vibra_shared PROPERTIES OUTPUT_NAME "vibra${SUFFIX}")
set_target_properties(vibra_static PROPERTIES OUTPUT_NAME "vibra${SUFFIX}")

# Install library files to standard system paths
install(TARGETS vibra_shared vibra_static
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install header files to the include directory
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
