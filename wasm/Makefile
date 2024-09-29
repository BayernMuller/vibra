# Makefile for building a WebAssembly module using Emscripten

EMSDK_PATH := $(EMSDK)
EMCC := $(EMSDK_PATH)/upstream/emscripten/emcc
EMCXX := $(EMSDK_PATH)/upstream/emscripten/em++

SRC_DIR := .
BUILD_DIR := build

SOURCES := $(SRC_DIR)/wasm.cpp

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
TARGET_JS := $(BUILD_DIR)/vibra.js

WASM_SETTINGS := -s EXPORTED_FUNCTIONS="['_GetWavSignature', '_GetFingerprint', '_GetPcmSignature', '_GetSampleMs', '_malloc', '_free']" \
                 -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
                 -s ALLOW_MEMORY_GROWTH=1 \
                 -s WASM=1

CFLAGS := -Os
CXXFLAGS := -Os -std=c++11 -I/usr/local/include
LDFLAGS := $(WASM_SETTINGS) -L/usr/local/lib -lvibra -lfftw3

all: $(TARGET_JS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(EMCXX) $(CXXFLAGS) -c $< -o $@

$(TARGET_JS): $(OBJECTS)
	$(EMCXX) $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean