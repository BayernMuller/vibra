#include <gtest/gtest.h>

#include <array>

#include "utils/fft.h"

namespace fft = vibra::fft;

TEST(FFTTest, ReturnsMinimumMagnitudeForSilence) {
  fft::FFT<8> fft;
  const std::array<double, 8> input = {
      {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}};

  const auto output = fft.RFFT(input);

  ASSERT_EQ(output.size(), 5u);
  for (const auto value : output) {
    EXPECT_DOUBLE_EQ(static_cast<double>(value), 1e-10);
  }
}

TEST(FFTTest, PlacesConstantSignalEnergyInDcBin) {
  fft::FFT<8> fft;
  const std::array<double, 8> input = {
      {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}};

  const auto output = fft.RFFT(input);

  EXPECT_NEAR(static_cast<double>(output[0]), 64.0 / 131072.0, 1e-12);
  for (std::size_t i = 1; i < output.size(); ++i) {
    EXPECT_DOUBLE_EQ(static_cast<double>(output[i]), 1e-10);
  }
}
