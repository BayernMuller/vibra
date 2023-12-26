CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -O2
LDFLAGS = -lm -lfftw3

AUDIO_FILE_TO_FINGERPRINT_SOURCES = audio_file_to_fingerprint.cpp \
                                    ../fingerprinting/algorithm/signature_generator.cpp \
                                    ../fingerprinting/audio/wav.cpp \
                                    ../fingerprinting/utils/fft.cpp \
                                    ../fingerprinting/utils/ring_buffer.cpp \
                                    ../fingerprinting/algorithm/signature.cpp \
									../fingerprinting/algorithm/frequency.cpp 

AUDIO_FILE_TO_FINGERPRINT_OBJECTS = $(AUDIO_FILE_TO_FINGERPRINT_SOURCES:.cpp=.o)

all: audio_file_to_fingerprint

audio_file_to_fingerprint: $(AUDIO_FILE_TO_FINGERPRINT_OBJECTS)
	$(CXX) $(LDFLAGS) $(AUDIO_FILE_TO_FINGERPRINT_OBJECTS) -o $@

clean:
	rm -f $(AUDIO_FILE_TO_FINGERPRINT_OBJECTS) audio_file_to_fingerprint

.PHONY: all clean
