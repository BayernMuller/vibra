#include <gtest/gtest.h>

// utils/uuid4
#include "../lib/utils/uuid4.h"

TEST(UUID, generate)
{
    std::string uuid = uuid4::generate();
    EXPECT_EQ(uuid.size(), 36);
}

// utils/crc32
#include "../lib/utils/crc32.h"

TEST(CRC32, crc32)
{
    std::string data = "Hello, World!";
    std::uint32_t crc32 = crc32::crc32(data.c_str(), data.size());
    EXPECT_EQ(crc32, 0xec4ac3d0);
}

// utils/fft
#include "../lib/utils/fft.h"

TEST(FFT, rfft)
{
    std::vector<double> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    fft::RealArray fft = fft::FFT::RFFT(data);
    EXPECT_EQ(fft.size(), 6);

    EXPECT_NEAR(fft[0], 0.0230789, 1e-5);
    EXPECT_NEAR(fft[1], 0.0019974, 1e-5);
    EXPECT_NEAR(fft[2], 0.0005520, 1e-5);
    EXPECT_NEAR(fft[3], 0.0002914, 1e-5);
    EXPECT_NEAR(fft[4], 0.0002108, 1e-5);
    EXPECT_NEAR(fft[5], 0.0001907, 1e-5);
}

// utils/base64
#include "../lib/utils/base64.h"

TEST(Base64, encode)
{
    std::string data = "FC Bayern is the best club in the world!    ";
    std::string encoded = base64::encode(data.c_str(), data.size());
    EXPECT_EQ(encoded, "RkMgQmF5ZXJuIGlzIHRoZSBiZXN0IGNsdWIgaW4gdGhlIHdvcmxkISAgICA=");
}

// utils/ring_buffer
#include "../lib/utils/ring_buffer.h"

TEST(RingBuffer, Append)
{
    RingBuffer<int> buffer(10);
    for (int i = 0; i < 15; ++i)
    {
        buffer.Append(i);
    }
    EXPECT_EQ(buffer.size(), 10);
    EXPECT_EQ(buffer.num_written(), 15);
    EXPECT_EQ(buffer.position(), 5);
    EXPECT_EQ(buffer[0], 10);
    EXPECT_EQ(buffer[1], 11);
    EXPECT_EQ(buffer[2], 12);
    EXPECT_EQ(buffer[3], 13);
    EXPECT_EQ(buffer[4], 14);
}

// audio/wav
#include "../lib/audio/wav.h"

TEST(Wav, FromSignedPCM)
{
    std::vector<char> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Wav wav = Wav::FromSignedPCM(data.data(), data.size(), 44100, 16, 2);
    EXPECT_EQ(wav.audio_format(), 1);
    EXPECT_EQ(wav.num_channels(), 2);
    EXPECT_EQ(wav.sample_rate_(), 44100);
    EXPECT_EQ(wav.bits_per_sample(), 16);
    EXPECT_EQ(wav.data_size(), 10);
    EXPECT_TRUE(std::equal(wav.data().get(), wav.data().get() + wav.data_size(), data.begin()));
}

TEST(Wav, FromFloatPCM)
{
    std::vector<float> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Wav wav = Wav::FromFloatPCM(reinterpret_cast<const char *>(data.data()),
                                data.size() * sizeof(float), 44100, sizeof(float) * 8, 2);
    EXPECT_EQ(wav.audio_format(), 3);
    EXPECT_EQ(wav.num_channels(), 2);
    EXPECT_EQ(wav.sample_rate_(), 44100);
    EXPECT_EQ(wav.bits_per_sample(), sizeof(float) * 8);
    EXPECT_EQ(wav.data_size(), data.size() * sizeof(float));
    EXPECT_TRUE(
        std::equal(reinterpret_cast<float *>(wav.data().get()),
                   reinterpret_cast<float *>(wav.data().get()) + wav.data_size() / sizeof(float),
                   data.begin()));
}

