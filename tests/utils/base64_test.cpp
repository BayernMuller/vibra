#include <gtest/gtest.h>

#include "utils/base64.h"

namespace base64 = vibra::base64;

TEST(Base64Test, EncodesEmptyInput) {
  EXPECT_EQ(base64::Encode("", 0), "");
}

TEST(Base64Test, EncodesInputsWithPadding) {
  EXPECT_EQ(base64::Encode("f", 1), "Zg==");
  EXPECT_EQ(base64::Encode("fo", 2), "Zm8=");
}

TEST(Base64Test, EncodesInputWithoutPadding) {
  EXPECT_EQ(base64::Encode("foo", 3), "Zm9v");
  EXPECT_EQ(base64::Encode("hello", 5), "aGVsbG8=");
}
