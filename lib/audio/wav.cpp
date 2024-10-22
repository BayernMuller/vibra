#include "audio/wav.h"
#include <cmath>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>

Wav Wav::FromFile(const std::string& wav_file_path)
{
    Wav wav;
    wav.mWavFilePath = wav_file_path;
    std::ifstream stream(wav_file_path, std::ios::binary);
    if (!stream.is_open())
    {
        throw std::runtime_error("Failed to open WAV file");
    }
    wav.readWavFileBuffer(stream);
    return wav;
}

Wav Wav::FromRawWav(const char* raw_wav, std::uint32_t raw_wav_size)
{
    Wav wav;
    std::istringstream stream(std::string(raw_wav, raw_wav_size));
    wav.readWavFileBuffer(stream);
    return wav;
}

Wav Wav::FromSignedPCM(const char* raw_pcm, std::uint32_t raw_pcm_size,
                    std::uint32_t sample_rate, std::uint32_t sample_width,
                    std::uint32_t channel_count)
{
    return fromPCM(
        raw_pcm,
        raw_pcm_size,
        AudioFormat::PCM_INTEGER,
        sample_rate,
        sample_width,
        channel_count
    );
}

Wav Wav::FromFloatPCM(const char* raw_pcm, std::uint32_t raw_pcm_size,
                    std::uint32_t sample_rate, std::uint32_t sample_width,
                    std::uint32_t channel_count)
{
    return fromPCM(
        raw_pcm,
        raw_pcm_size,
        AudioFormat::PCM_FLOAT,
        sample_rate,
        sample_width,
        channel_count
    );
}

Wav::~Wav()
{
}

Wav Wav::fromPCM(const char* raw_pcm, std::uint32_t raw_pcm_size,
                AudioFormat audio_format, std::uint32_t sample_rate,
                std::uint32_t sample_width, std::uint32_t channel_count)
{
    Wav wav;
    wav.mHeader.file_size = sizeof(WavHeader) + sizeof(FmtSubchunk) + 8 + raw_pcm_size;
    wav.mFmt.audio_format = static_cast<std::uint16_t>(audio_format);
    wav.mFmt.num_channels = channel_count;
    wav.mFmt.sample_rate = sample_rate;
    wav.mFmt.byte_rate = sample_rate * channel_count * sample_width / 8;
    wav.mFmt.block_align = channel_count * sample_width / 8;
    wav.mFmt.bits_per_sample = sample_width;
    wav.mDataSize = raw_pcm_size;
    wav.mData.reset(new std::uint8_t[raw_pcm_size]);
    std::memcpy(wav.mData.get(), raw_pcm, raw_pcm_size);
    return wav;
}

void Wav::readWavFileBuffer(std::istream& stream)
{
    stream.read(reinterpret_cast<char*>(&mHeader), sizeof(WavHeader));

    const auto SUBCHUNK_LIMIT = 10;

    bool data_chunk_found = false;
    bool fmt_chunk_found = false;
    for (int i = 0; i < SUBCHUNK_LIMIT && stream.tellg() < mHeader.file_size - 8; i++)
    {
        char subchunk_id[4];
        stream.read(subchunk_id, 4);

        std::uint32_t subchunk_size;
        stream.read(reinterpret_cast<char*>(&subchunk_size), 4);

        if (strncmp(subchunk_id, "data", 4) == 0)
        {
            mDataSize = subchunk_size;
            mData.reset(new std::uint8_t[mDataSize]);
            stream.read(reinterpret_cast<char*>(mData.get()), mDataSize);
            data_chunk_found = true;
        }
        else if (strncmp(subchunk_id, "fmt ", 4) == 0)
        {
            stream.read(reinterpret_cast<char*>(&mFmt), sizeof(FmtSubchunk));
            fmt_chunk_found = true;
        }
        else
        {
            stream.seekg(subchunk_size, std::ios::cur);
        }

        if (data_chunk_found && fmt_chunk_found)
        {
            return; // read wav successfully
        }
    }

    if (!data_chunk_found || !fmt_chunk_found)
    {
        throw std::runtime_error("Invalid WAV file");
    }
}
