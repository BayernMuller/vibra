# Define the sources for the library
set(LIBVIBRA_SOURCES
    vibra.cpp
    algorithm/signature.cpp
    algorithm/frequency.cpp
    algorithm/signature_generator.cpp
    audio/wav.cpp
    audio/downsampler.cpp
    communication/shazam.cpp
)

# Add shared and static libraries for libvibra
add_library(vibra_shared SHARED ${LIBVIBRA_SOURCES})
add_library(vibra_static STATIC ${LIBVIBRA_SOURCES})

if (DEFINED FFTW3_PATH)
    set(FFTW3_INCLUDE_DIR ${FFTW3_PATH}/include)
    set(FFTW3_LIBRARY ${FFTW3_PATH}/lib/libfftw3.a)
    target_include_directories(vibra_shared PRIVATE ${FFTW3_INCLUDE_DIR})
    target_include_directories(vibra_static PRIVATE ${FFTW3_INCLUDE_DIR})
else()
    find_library(FFTW3_LIBRARY NAMES fftw3)
endif()

target_link_libraries(vibra_shared PRIVATE ${FFTW3_LIBRARY})
target_link_libraries(vibra_static PRIVATE ${FFTW3_LIBRARY})

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