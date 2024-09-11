# Makefile for building a WebAssembly module using Emscripten

EMSDK_PATH := $(EMSDK)
EMCC := $(EMSDK_PATH)/upstream/emscripten/emcc
EMCXX := $(EMSDK_PATH)/upstream/emscripten/em++

FFTW3_PATH := $(FFTW3_PATH)
FFTW3_INCLUDE := $(FFTW3_PATH)/include
FFTW3_LIB := $(FFTW3_PATH)/lib

SRC_DIR := ..
BUILD_DIR := build

SOURCES := $(SRC_DIR)/wasm/wasm.cpp \
           $(SRC_DIR)/fingerprinting/algorithm/signature.cpp \
           $(SRC_DIR)/fingerprinting/algorithm/frequency.cpp \
           $(SRC_DIR)/fingerprinting/algorithm/signature_generator.cpp \
           $(SRC_DIR)/fingerprinting/audio/wav.cpp

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
TARGET_JS := $(BUILD_DIR)/vibra.js

WASM_SETTINGS := -s EXPORTED_FUNCTIONS="['_GetWavSignature', '_GetFingerprint', '_GetPcmSignature', '_GetSampleMs', '_malloc', '_free']" \
                 -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" \
                 -s ALLOW_MEMORY_GROWTH=1 \
                 -s WASM=1

CFLAGS := -Os
CXXFLAGS := -Os -std=c++11 -I$(FFTW3_INCLUDE)
LDFLAGS := $(WASM_SETTINGS) -L$(FFTW3_LIB) -lfftw3

all: $(TARGET_JS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(EMCXX) $(CXXFLAGS) -c $< -o $@

$(TARGET_JS): $(OBJECTS)
	$(EMCXX) $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean