#include <gtest/gtest.h>

#include <string>

#include "utils/crc32.h"

namespace crc32 = vibra::crc32;

TEST(Crc32Test, ReturnsKnownChecksum) {
  const std::string input = "123456789";

  EXPECT_EQ(crc32::Crc32(input.data(), input.size()), 0xCBF43926u);
}

TEST(Crc32Test, ReturnsZeroForEmptyBuffer) {
  EXPECT_EQ(crc32::Crc32("", 0), 0u);
}
