#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "audio/wav.h"

using vibra::AudioFormat;
using vibra::FmtSubchunk;
using vibra::Wav;

namespace {
void Append(std::string* out, const char* bytes, std::size_t size) {
  out->append(bytes, size);
}

template <typename T>
void AppendLittleEndian(std::string* out, T value) {
  for (std::size_t i = 0; i < sizeof(T); ++i) {
    out->push_back(static_cast<char>(value >> (i * 8)));
  }
}

std::string MakeRawWav(const std::vector<std::int16_t>& samples) {
  std::string raw;
  const std::uint32_t data_size = samples.size() * sizeof(std::int16_t);

  Append(&raw, "RIFF", 4);
  AppendLittleEndian<std::uint32_t>(&raw, 36 + data_size);
  Append(&raw, "WAVE", 4);
  Append(&raw, "fmt ", 4);
  AppendLittleEndian<std::uint32_t>(&raw, sizeof(FmtSubchunk));
  AppendLittleEndian<std::uint16_t>(
      &raw, static_cast<std::uint16_t>(AudioFormat::kPcmInteger));
  AppendLittleEndian<std::uint16_t>(&raw, 1);
  AppendLittleEndian<std::uint32_t>(&raw, 16000);
  AppendLittleEndian<std::uint32_t>(&raw, 16000 * sizeof(std::int16_t));
  AppendLittleEndian<std::uint16_t>(&raw, sizeof(std::int16_t));
  AppendLittleEndian<std::uint16_t>(&raw, 16);
  Append(&raw, "data", 4);
  AppendLittleEndian<std::uint32_t>(&raw, data_size);
  Append(&raw, reinterpret_cast<const char*>(samples.data()), data_size);
  return raw;
}
}  // namespace

TEST(WavTest, CreatesSignedPcmWavMetadata) {
  const std::int16_t samples[] = {100, -200};
  const Wav wav = Wav::FromSignedPCM(reinterpret_cast<const char*>(samples),
                                     sizeof(samples), 44100, 16, 2);

  EXPECT_EQ(wav.audio_format(),
            static_cast<std::uint16_t>(AudioFormat::kPcmInteger));
  EXPECT_EQ(wav.num_channels(), 2);
  EXPECT_EQ(wav.sample_rate_(), 44100u);
  EXPECT_EQ(wav.bits_per_sample(), 16u);
  EXPECT_EQ(wav.data_size(), sizeof(samples));
}

TEST(WavTest, CreatesFloatPcmWavMetadata) {
  const float samples[] = {0.25f, -0.25f};
  const Wav wav = Wav::FromFloatPCM(reinterpret_cast<const char*>(samples),
                                    sizeof(samples), 48000, 32, 1);

  EXPECT_EQ(wav.audio_format(),
            static_cast<std::uint16_t>(AudioFormat::kPcmFloat));
  EXPECT_EQ(wav.num_channels(), 1);
  EXPECT_EQ(wav.sample_rate_(), 48000u);
  EXPECT_EQ(wav.bits_per_sample(), 32u);
  EXPECT_EQ(wav.data_size(), sizeof(samples));
}

TEST(WavTest, ParsesRawWavData) {
  const std::vector<std::int16_t> samples = {100, -200, 300};
  const std::string raw = MakeRawWav(samples);

  const Wav wav = Wav::FromRawWav(raw.data(), raw.size());

  EXPECT_EQ(wav.audio_format(),
            static_cast<std::uint16_t>(AudioFormat::kPcmInteger));
  EXPECT_EQ(wav.num_channels(), 1);
  EXPECT_EQ(wav.sample_rate_(), 16000u);
  EXPECT_EQ(wav.bits_per_sample(), 16u);
  EXPECT_EQ(wav.data_size(), samples.size() * sizeof(std::int16_t));
  EXPECT_EQ(reinterpret_cast<const std::int16_t*>(wav.data())[1], -200);
}

TEST(WavTest, RejectsInvalidRawWavData) {
  const std::string raw = "not a wav";

  EXPECT_THROW(Wav::FromRawWav(raw.data(), raw.size()), std::runtime_error);
}
