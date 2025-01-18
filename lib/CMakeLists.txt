# Define the sources for the library
set(LIBVIBRA_SOURCES
    vibra.cpp
    algorithm/signature.cpp
    algorithm/frequency.cpp
    algorithm/signature_generator.cpp
    audio/wav.cpp
    audio/downsampler.cpp
)

# Add shared and static libraries for libvibra
add_library(vibra_shared SHARED ${LIBVIBRA_SOURCES})
add_library(vibra_static STATIC ${LIBVIBRA_SOURCES})

target_include_directories(vibra_shared PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(vibra_static PRIVATE ${CMAKE_SOURCE_DIR}/include)

if (DEFINED FFTW3_PATH)
    set(FFTW3_INCLUDE_DIR ${FFTW3_PATH}/include)
    set(FFTW3_LIBRARY ${FFTW3_PATH}/lib/libfftw3.a)
else()
    find_path(FFTW3_INCLUDE_DIR fftw3.h)
    find_library(FFTW3_LIBRARY NAMES fftw3)
endif()
if (NOT FFTW3_LIBRARY OR NOT FFTW3_INCLUDE_DIR)
    message(FATAL_ERROR "FFTW3 library or include path not found. Please install FFTW3 or specify FFTW3_PATH.")
endif()

message(STATUS "FFTW3_INCLUDE_DIR: ${FFTW3_INCLUDE_DIR}")
message(STATUS "FFTW3_LIBRARY: ${FFTW3_LIBRARY}")
target_include_directories(vibra_shared PRIVATE ${FFTW3_INCLUDE_DIR})
target_include_directories(vibra_static PRIVATE ${FFTW3_INCLUDE_DIR})
target_link_libraries(vibra_shared PRIVATE ${FFTW3_LIBRARY})
target_link_libraries(vibra_static PRIVATE ${FFTW3_LIBRARY})

# Set C++11 standard
set_target_properties(vibra_shared PROPERTIES CXX_STANDARD 11)
set_target_properties(vibra_static PROPERTIES CXX_STANDARD 11)

# Set output names
set_target_properties(vibra_shared PROPERTIES OUTPUT_NAME "vibra")
set_target_properties(vibra_static PROPERTIES OUTPUT_NAME "vibra")

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