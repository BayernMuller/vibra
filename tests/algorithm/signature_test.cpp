#include <gtest/gtest.h>

#include <string>

#include "algorithm/signature.h"

using vibra::FrequencyBand;
using vibra::FrequencyPeak;
using vibra::Signature;

TEST(SignatureTest, TracksSampleMetadataAndPeakCounts) {
  Signature signature({16000, 128});

  signature.AddNumSamples(256);
  signature.frequency_band_to_peaks()[FrequencyBand::k250To520].push_back(
      FrequencyPeak({1, 6000, 2048, 16000}));
  signature.frequency_band_to_peaks()[FrequencyBand::k520To1450].push_back(
      FrequencyPeak({2, 6100, 4096, 16000}));

  EXPECT_EQ(signature.sample_rate(), 16000u);
  EXPECT_EQ(signature.num_samples(), 384u);
  EXPECT_EQ(signature.SumOfPeaksLength(), 2u);
}

TEST(SignatureTest, ResetClearsPeaksAndUpdatesMetadata) {
  Signature signature({16000, 128});
  signature.frequency_band_to_peaks()[FrequencyBand::k250To520].push_back(
      FrequencyPeak({1, 6000, 2048, 16000}));

  signature.Reset({8000, 64});

  EXPECT_EQ(signature.sample_rate(), 8000u);
  EXPECT_EQ(signature.num_samples(), 64u);
  EXPECT_EQ(signature.SumOfPeaksLength(), 0u);
  EXPECT_TRUE(signature.frequency_band_to_peaks().empty());
}

TEST(SignatureTest, EncodesAsShazamDataUri) {
  Signature signature({16000, 128});
  const std::string prefix = "data:audio/vnd.shazam.sig;base64,";
  signature.frequency_band_to_peaks()[FrequencyBand::k250To520].push_back(
      FrequencyPeak({1, 6144, 2048, 16000}));

  const std::string encoded = signature.EncodeBase64();

  EXPECT_EQ(encoded.find(prefix), 0u);
  EXPECT_GT(encoded.size(), prefix.size());
}
