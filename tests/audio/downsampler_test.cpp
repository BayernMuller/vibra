#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <vector>

#include "audio/downsampler.h"
#include "audio/wav.h"

using vibra::Downsampler;
using vibra::kLowQualitySampleBitWidth;
using vibra::kLowQualitySampleRate;
using vibra::LowQualityTrack;
using vibra::Wav;

namespace {
constexpr double kPi = 3.14159265358979323846;

std::vector<std::int16_t> MakeSineWave(std::uint32_t sample_rate,
                                       double seconds, double frequency_hz) {
  const std::uint32_t sample_count = sample_rate * seconds;
  std::vector<std::int16_t> samples(sample_count);
  for (std::uint32_t i = 0; i < sample_count; i++) {
    const double phase = 2.0 * kPi * frequency_hz * i / sample_rate;
    samples[i] = static_cast<std::int16_t>(std::sin(phase) * 12000.0);
  }
  return samples;
}
}  // namespace

TEST(DownsamplerTest, CopiesAlreadyLowQualityMonoPcm) {
  const std::int16_t samples[] = {100, -200, 300};
  const Wav wav = Wav::FromSignedPCM(reinterpret_cast<const char*>(samples),
                                     sizeof(samples), kLowQualitySampleRate,
                                     kLowQualitySampleBitWidth, 1);

  const LowQualityTrack downsampled = Downsampler::GetLowQualityPCM(wav);

  ASSERT_EQ(downsampled.size(), 3u);
  EXPECT_EQ(downsampled[0], 100);
  EXPECT_EQ(downsampled[1], -200);
  EXPECT_EQ(downsampled[2], 300);
}

TEST(DownsamplerTest, MixesSignedStereoToMono) {
  const std::int16_t samples[] = {1000, 3000, -1000, 1000};
  const Wav wav = Wav::FromSignedPCM(reinterpret_cast<const char*>(samples),
                                     sizeof(samples), kLowQualitySampleRate,
                                     kLowQualitySampleBitWidth, 2);

  const LowQualityTrack downsampled = Downsampler::GetLowQualityPCM(wav);

  ASSERT_EQ(downsampled.size(), 2u);
  EXPECT_EQ(downsampled[0], 2000);
  EXPECT_EQ(downsampled[1], 0);
}

TEST(DownsamplerTest, DownsamplesSignedMonoByRatio) {
  const std::int16_t samples[] = {1, 2, 3, 4};
  const Wav wav = Wav::FromSignedPCM(reinterpret_cast<const char*>(samples),
                                     sizeof(samples), kLowQualitySampleRate * 2,
                                     kLowQualitySampleBitWidth, 1);

  const LowQualityTrack downsampled = Downsampler::GetLowQualityPCM(wav);

  ASSERT_EQ(downsampled.size(), 2u);
  EXPECT_EQ(downsampled[0], 1);
  EXPECT_EQ(downsampled[1], 3);
}

TEST(DownsamplerTest, ResamplesSignedMonoToMatchEquivalentLowQualityTone) {
  const std::vector<std::int16_t> reference =
      MakeSineWave(kLowQualitySampleRate, 0.1, 440.0);
  const std::vector<std::int16_t> source = MakeSineWave(44100, 0.1, 440.0);
  const Wav wav =
      Wav::FromSignedPCM(reinterpret_cast<const char*>(source.data()),
                         source.size() * sizeof(std::int16_t), 44100,
                         kLowQualitySampleBitWidth, 1);

  const LowQualityTrack downsampled = Downsampler::GetLowQualityPCM(wav);

  ASSERT_EQ(downsampled.size(), reference.size());

  double mean_absolute_error = 0.0;
  for (std::size_t i = 0; i < reference.size(); i++) {
    mean_absolute_error += std::abs(downsampled[i] - reference[i]);
  }
  mean_absolute_error /= reference.size();

  EXPECT_LT(mean_absolute_error, 10.0);
}

TEST(DownsamplerTest, ConvertsFloatMonoToSignedLowQualityPcm) {
  const float samples[] = {0.5f, -0.5f};
  const Wav wav =
      Wav::FromFloatPCM(reinterpret_cast<const char*>(samples), sizeof(samples),
                        kLowQualitySampleRate, 32, 1);

  const LowQualityTrack downsampled = Downsampler::GetLowQualityPCM(wav);

  ASSERT_EQ(downsampled.size(), 2u);
  EXPECT_EQ(downsampled[0], 16383);
  EXPECT_EQ(downsampled[1], -16383);
}
