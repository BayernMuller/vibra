#include <gtest/gtest.h>

#include <cmath>

#include "algorithm/frequency.h"

using vibra::FrequencyPeak;

TEST(FrequencyPeakTest, ExposesConstructorValues) {
  FrequencyPeak peak({42, 6144, 2048, 16000});

  EXPECT_EQ(peak.fft_pass_number(), 42u);
  EXPECT_EQ(peak.peak_magnitude(), 6144u);
  EXPECT_EQ(peak.corrected_peak_frequency_bin(), 2048u);
}

TEST(FrequencyPeakTest, ComputesFrequencyAmplitudeAndElapsedSeconds) {
  FrequencyPeak peak({10, 6144, 1024, 16000});

  EXPECT_DOUBLE_EQ(peak.ComputeFrequency(), 125.0);
  EXPECT_DOUBLE_EQ(peak.ComputeElapsedSeconds(), 0.08);
  EXPECT_DOUBLE_EQ(peak.ComputeAmplitudePCM(), std::sqrt(65536.0) / 1024.0);
}
