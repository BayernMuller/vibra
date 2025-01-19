# Fetch the args header-only library
include(FetchContent)
FetchContent_Declare(
    args
    GIT_REPOSITORY https://github.com/Taywee/args.git
    GIT_TAG        master
)
FetchContent_Populate(args)

# Add the executable for the CLI
add_executable(vibra
    main.cpp
    cli.cpp
    communication/shazam.cpp
)

# Add the args include directory to the target
target_include_directories(vibra PRIVATE ${args_SOURCE_DIR})

# Add the include directory to the target
target_include_directories(vibra PRIVATE ${CMAKE_SOURCE_DIR}/cli)

# Link the CLI with the static libvibra and libcurl
if(MSVC)
    find_package(CURL REQUIRED)
    target_link_libraries(vibra PRIVATE vibra_static CURL::libcurl)
else()
    find_library(CURL_LIBRARY NAMES curl)
    target_link_libraries(vibra PRIVATE vibra_static ${CURL_LIBRARY})
endif()

# Set the output name
if (DEFINED BUILD_SUFFIX)
    message(STATUS "Adding suffix: ${BUILD_SUFFIX}")
    set(SUFFIX "-${BUILD_SUFFIX}")
else()
    set(SUFFIX "")
endif()
set_target_properties(vibra PROPERTIES OUTPUT_NAME "vibra${SUFFIX}")

# Install the CLI
install(TARGETS vibra
    RUNTIME DESTINATION bin
)