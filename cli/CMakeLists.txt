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

# Find libcurl
find_library(CURL_LIBRARY NAMES curl)

# Link the CLI with the static libvibra and libcurl
target_link_libraries(vibra PRIVATE vibra_static ${CURL_LIBRARY})

# Install the CLI
install(TARGETS vibra
    RUNTIME DESTINATION bin
)