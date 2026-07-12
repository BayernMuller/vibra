#include <gtest/gtest.h>

#include <cstdint>

#include "utils/ring_buffer.h"

using vibra::RingBuffer;

TEST(RingBufferTest, InitializesWithDefaultValue) {
  RingBuffer<std::int32_t> buffer(3, 7);

  EXPECT_EQ(buffer.size(), 3u);
  EXPECT_EQ(buffer.num_written(), 0u);
  EXPECT_EQ(buffer.position(), 0u);
  EXPECT_EQ(buffer[0], 7);
  EXPECT_EQ(buffer[1], 7);
  EXPECT_EQ(buffer[2], 7);
}

TEST(RingBufferTest, AppendsAndWrapsPosition) {
  RingBuffer<std::int32_t> buffer(3, 0);

  buffer.Append(1);
  buffer.Append(2);
  buffer.Append(3);

  EXPECT_EQ(buffer[0], 1);
  EXPECT_EQ(buffer[1], 2);
  EXPECT_EQ(buffer[2], 3);
  EXPECT_EQ(buffer.position(), 0u);
  EXPECT_EQ(buffer.num_written(), 3u);

  buffer.Append(4);

  EXPECT_EQ(buffer[0], 4);
  EXPECT_EQ(buffer[1], 2);
  EXPECT_EQ(buffer[2], 3);
  EXPECT_EQ(buffer.position(), 1u);
  EXPECT_EQ(buffer.num_written(), 4u);
}

TEST(RingBufferTest, SupportsNegativeIndexFromBack) {
  RingBuffer<std::int32_t> buffer(3, 0);

  buffer.Append(10);
  buffer.Append(20);
  buffer.Append(30);

  EXPECT_EQ(buffer[-1], 30);
  EXPECT_EQ(buffer[-2], 20);
  EXPECT_EQ(buffer[-3], 10);
}

TEST(RingBufferTest, WrapsSignedPositionOffset) {
  RingBuffer<std::int32_t> buffer(3, 0);

  buffer.Append(10);
  buffer.Append(20);
  buffer.Append(30);

  const std::uint32_t position = 0;

  EXPECT_EQ(buffer[static_cast<std::int32_t>(position) - 1], 30);
}
