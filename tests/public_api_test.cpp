#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "include/vibra.h"

namespace {
void Append(std::string* out, const char* bytes, std::size_t size) {
  out->append(bytes, size);
}

template <typename T>
void AppendLittleEndian(std::string* out, T value) {
  for (std::size_t i = 0; i < sizeof(T); i++) {
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
  AppendLittleEndian<std::uint32_t>(&raw, 16);
  AppendLittleEndian<std::uint16_t>(&raw, 1);
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

TEST(PublicApiTest, GeneratesAndFreesFingerprintFromSignedPcm) {
  const std::vector<std::int16_t> samples(128, 0);
  const std::string prefix = "data:audio/vnd.shazam.sig;base64,";

  Fingerprint* fingerprint = vibra_get_fingerprint_from_signed_pcm(
      reinterpret_cast<const char*>(samples.data()),
      samples.size() * sizeof(std::int16_t), 16000, 16, 1);

  ASSERT_NE(fingerprint, nullptr);
  EXPECT_EQ(vibra_get_sample_ms_from_fingerprint(fingerprint), 8u);
  EXPECT_EQ(
      std::string(vibra_get_uri_from_fingerprint(fingerprint)).find(prefix),
      0u);

  vibra_free_fingerprint(fingerprint);
}

TEST(PublicApiTest, GeneratesSameFingerprintForEquivalentLowQualityWavAndPcm) {
  std::vector<std::int16_t> samples(512);
  for (std::size_t i = 0; i < samples.size(); i++) {
    samples[i] = static_cast<std::int16_t>(i % 64);
  }
  const std::string raw_wav = MakeRawWav(samples);

  Fingerprint* wav_fingerprint = vibra_get_fingerprint_from_wav_data(
      raw_wav.data(), static_cast<int>(raw_wav.size()));
  Fingerprint* pcm_fingerprint = vibra_get_fingerprint_from_signed_pcm(
      reinterpret_cast<const char*>(samples.data()),
      samples.size() * sizeof(std::int16_t), 16000, 16, 1);

  ASSERT_NE(wav_fingerprint, nullptr);
  ASSERT_NE(pcm_fingerprint, nullptr);
  EXPECT_EQ(vibra_get_sample_ms_from_fingerprint(wav_fingerprint),
            vibra_get_sample_ms_from_fingerprint(pcm_fingerprint));
  EXPECT_STREQ(vibra_get_uri_from_fingerprint(wav_fingerprint),
               vibra_get_uri_from_fingerprint(pcm_fingerprint));

  vibra_free_fingerprint(wav_fingerprint);
  vibra_free_fingerprint(pcm_fingerprint);
}

TEST(PublicApiTest, GeneratesFingerprintFromFloatPcm) {
  const std::vector<float> samples(128, 0.0f);

  Fingerprint* fingerprint = vibra_get_fingerprint_from_float_pcm(
      reinterpret_cast<const char*>(samples.data()),
      samples.size() * sizeof(float), 16000, 32, 1);

  ASSERT_NE(fingerprint, nullptr);
  EXPECT_EQ(vibra_get_sample_ms_from_fingerprint(fingerprint), 8u);

  vibra_free_fingerprint(fingerprint);
}
