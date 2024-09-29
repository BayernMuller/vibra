# Add the executable for the CLI
add_executable(vibra
    main.cpp
    cli.cpp
)

# Find libcurl
find_library(CURL_LIBRARY NAMES curl)

# Link the CLI with the static libvibra and libcurl
target_link_libraries(vibra PRIVATE vibra_static ${CURL_LIBRARY})

# Install the CLI
install(TARGETS vibra
    RUNTIME DESTINATION bin
)