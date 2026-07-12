if(NOT EMSCRIPTEN)
    message(FATAL_ERROR "The JavaScript/WebAssembly bindings require Emscripten")
endif()

add_executable(vibra_wasm wasm.cpp)
add_executable(vibra_wasm_global wasm.cpp)

target_include_directories(vibra_wasm PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(vibra_wasm_global PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(vibra_wasm PRIVATE vibra_static)
target_link_libraries(vibra_wasm_global PRIVATE vibra_static)

set(VIBRA_WASM_LINK_OPTIONS
    "SHELL:-s EXPORTED_FUNCTIONS=['_GetWavSignature','_GetSignedPcmSignature','_GetFloatPcmSignature','_GetFingerprint','_GetSampleMs','_FreeFingerprint','_malloc','_free']"
    "SHELL:-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','HEAPU8']"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    "SHELL:-s WASM=1"
    "SHELL:-s ENVIRONMENT=web,worker"
)

target_link_options(vibra_wasm PRIVATE
    ${VIBRA_WASM_LINK_OPTIONS}
    "SHELL:-s MODULARIZE=1"
    "SHELL:-s EXPORT_ES6=1"
    "SHELL:-s EXPORT_NAME=createModule"
)

target_link_options(vibra_wasm_global PRIVATE
    ${VIBRA_WASM_LINK_OPTIONS}
)

set_target_properties(vibra_wasm PROPERTIES
    OUTPUT_NAME "vibra"
)

set_target_properties(vibra_wasm_global PROPERTIES
    OUTPUT_NAME "vibra"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/js/live-demo"
)
