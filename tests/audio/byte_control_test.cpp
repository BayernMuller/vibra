#include <gtest/gtest.h>

#include <cstdint>

#include "audio/byte_control.h"

TEST(ByteControlTest, ReadsLittleEndianIntegers) {
  const unsigned char bytes[] = {0x34, 0x12, 0x78, 0x56,
                                 0x00, 0x00, 0x00, 0x00};

  EXPECT_EQ(GETINT8(bytes, 0), 0x34);
  EXPECT_EQ(GETINT16(bytes, 0), 0x1234);
  EXPECT_EQ(GETINT32(bytes, 0), 0x56781234);
}

TEST(ByteControlTest, SignExtendsTwentyFourBitSamples) {
  const unsigned char positive[] = {0x00, 0x00, 0x01};
  const unsigned char negative[] = {0x00, 0x00, 0xFF};

  EXPECT_EQ(GETINT24(positive, 0), 65536);
  EXPECT_EQ(GETINT24(negative, 0), -65536);
}

TEST(ByteControlTest, ExpandsSamplesToSignedSixtyFourBitRange) {
  const std::int16_t sample16 = 1234;

  EXPECT_EQ(GETSAMPLE64(2, &sample16, 0),
            static_cast<std::int64_t>(sample16) * (1LL << 48));
}
