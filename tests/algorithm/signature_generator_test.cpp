#include <gtest/gtest.h>

#include <stdexcept>

#include "algorithm/signature_generator.h"

using vibra::kLowQualitySampleRate;
using vibra::LowQualityTrack;
using vibra::Signature;
using vibra::SignatureGenerator;

TEST(SignatureGeneratorTest, ThrowsWhenInputIsTooShort) {
  SignatureGenerator generator;

  EXPECT_THROW(generator.GetNextSignature(), std::runtime_error);
}

TEST(SignatureGeneratorTest, GeneratesSignatureFromOneChunk) {
  SignatureGenerator generator;
  generator.set_max_time_seconds(0.0);
  generator.FeedInput(LowQualityTrack(128, 0));

  const Signature signature = generator.GetNextSignature();

  EXPECT_EQ(signature.sample_rate(), kLowQualitySampleRate);
  EXPECT_EQ(signature.num_samples(), 128u);
}

TEST(SignatureGeneratorTest, AcceptsInputAcrossMultipleFeeds) {
  SignatureGenerator generator;
  generator.set_max_time_seconds(0.0);
  generator.FeedInput(LowQualityTrack(64, 0));
  generator.FeedInput(LowQualityTrack(64, 0));

  const Signature signature = generator.GetNextSignature();

  EXPECT_EQ(signature.num_samples(), 128u);
}
