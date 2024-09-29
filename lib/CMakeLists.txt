# Define the sources for the library
set(LIBVIBRA_SOURCES
    vibra.cpp
    algorithm/signature.cpp
    algorithm/frequency.cpp
    algorithm/signature_generator.cpp
    audio/wav.cpp
    communication/shazam.cpp
)

# Add shared and static libraries for libvibra
add_library(vibra_shared SHARED ${LIBVIBRA_SOURCES})
add_library(vibra_static STATIC ${LIBVIBRA_SOURCES})

# Find required libraries (FFTW3)
find_library(FFTW3_LIBRARY NAMES fftw3)

# Link FFTW3 library
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