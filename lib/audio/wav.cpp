#include "audio/wav.h"

#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include <fstream>
#include <iostream>
#include <new>
#include <sstream>
#include <string>

namespace vibra {

Wav Wav::FromFile(const std::string& wav_file_path) {
  Wav wav;
  wav.wav_file_path_ = wav_file_path;
  std::ifstream stream(wav_file_path, std::ios::binary);
  if (!stream.is_open()) {
    throw std::runtime_error("Failed to open WAV file");
  }
  wav.readWavFileBuffer(stream);
  return wav;
}

Wav Wav::FromRawWav(const char* raw_wav, std::uint32_t raw_wav_size) {
  Wav wav;
  std::istringstream stream(std::string(raw_wav, raw_wav_size));
  wav.readWavFileBuffer(stream);
  return wav;
}

Wav Wav::FromSignedPCM(const char* raw_pcm, std::uint32_t raw_pcm_size,
                       std::uint32_t sample_rate, std::uint32_t sample_width,
                       std::uint32_t channel_count) {
  return fromPCM(raw_pcm, raw_pcm_size, AudioFormat::kPcmInteger, sample_rate,
                 sample_width, channel_count);
}

Wav Wav::FromFloatPCM(const char* raw_pcm, std::uint32_t raw_pcm_size,
                      std::uint32_t sample_rate, std::uint32_t sample_width,
                      std::uint32_t channel_count) {
  return fromPCM(raw_pcm, raw_pcm_size, AudioFormat::kPcmFloat, sample_rate,
                 sample_width, channel_count);
}

Wav Wav::fromPCM(const char* raw_pcm, std::uint32_t raw_pcm_size,
                 AudioFormat audio_format, std::uint32_t sample_rate,
                 std::uint32_t sample_width, std::uint32_t channel_count) {
  Wav wav;
  wav.header_.file_size =
      sizeof(WavHeader) + sizeof(FmtSubchunk) + 8 + raw_pcm_size;
  wav.fmt_.audio_format = static_cast<std::uint16_t>(audio_format);
  wav.fmt_.num_channels = channel_count;
  wav.fmt_.sample_rate = sample_rate;
  wav.fmt_.byte_rate = sample_rate * channel_count * sample_width / 8;
  wav.fmt_.block_align = channel_count * sample_width / 8;
  wav.fmt_.bits_per_sample = sample_width;
  wav.data_size_ = raw_pcm_size;
  wav.data_.reset(allocateData(raw_pcm_size));
  std::memcpy(wav.data_.get(), raw_pcm, raw_pcm_size);
  return wav;
}

std::uint8_t* Wav::allocateData(std::uint32_t data_size) {
  auto* data = static_cast<std::uint8_t*>(std::malloc(data_size));
  if (data == nullptr && data_size != 0) {
    throw std::bad_alloc();
  }
  return data;
}

void Wav::readWavFileBuffer(std::istream& stream) {
  stream.read(reinterpret_cast<char*>(&header_), sizeof(WavHeader));

  const auto kSubchunkLimit = 10;

  bool data_chunk_found = false;
  bool fmt_chunk_found = false;
  for (int i = 0;
       i < kSubchunkLimit &&
       stream.tellg() < static_cast<std::streampos>(header_.file_size + 8);
       i++) {
    std::array<char, 4> subchunk_id = {};
    stream.read(subchunk_id.data(), subchunk_id.size());

    std::uint32_t subchunk_size;
    stream.read(reinterpret_cast<char*>(&subchunk_size), 4);

    if (strncmp(subchunk_id.data(), "data", subchunk_id.size()) == 0) {
      data_size_ = subchunk_size;
      data_.reset(allocateData(data_size_));
      stream.read(reinterpret_cast<char*>(data_.get()), data_size_);
      data_chunk_found = true;
    } else if (strncmp(subchunk_id.data(), "fmt ", subchunk_id.size()) == 0) {
      stream.read(reinterpret_cast<char*>(&fmt_), sizeof(FmtSubchunk));
      fmt_chunk_found = true;
    } else {
      stream.seekg(subchunk_size, std::ios::cur);
    }

    if (data_chunk_found && fmt_chunk_found) {
      return;  // read wav successfully
    }
  }

  if (!data_chunk_found || !fmt_chunk_found) {
    throw std::runtime_error("Invalid WAV file");
  }
}

}  // namespace vibra
