#include "wav.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <memory.h>
#include <sstream>  
#include <algorithm>

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

Raw16bitPCM Wav::GetLowQualityPCM(std::int32_t start_sec, std::int32_t end_sec) const
{
    Raw16bitPCM raw_pcm;

    if (mFmt.num_channels == 1 && 
        mFmt.sample_rate == LOW_QUALITY_SAMPLE_RATE && 
        mFmt.bits_per_sample == 16 && 
        start_sec == 0 && 
        end_sec == -1)
    {
        // no need to convert low quality pcm. just copy raw data
        raw_pcm.resize(mDataSize);
        ::memcpy(raw_pcm.data(), mData.get(), mDataSize);
        return raw_pcm;
    }
    
    
    double downsample_ratio = mFmt.sample_rate / (double)LOW_QUALITY_SAMPLE_RATE;
    std::uint32_t width = mFmt.bits_per_sample / 8;
    std::uint32_t sample_count = mDataSize / width;

    const void* raw_data = mData.get() + (start_sec * mFmt.sample_rate * width * mFmt.num_channels);

    std::uint32_t new_sample_count = sample_count / mFmt.num_channels / downsample_ratio;

    if (end_sec != -1)
    {
        new_sample_count = (end_sec - start_sec) * LOW_QUALITY_SAMPLE_RATE;
    }

    raw_pcm.resize(new_sample_count);

    auto getMonoSample = &Wav::getMonoSample;
    if (mFmt.num_channels == 1)
    {
        getMonoSample = &Wav::monoToMonoSample;
    }
    else if (mFmt.num_channels == 2)
    {
        getMonoSample = &Wav::stereoToMonoSample;
    } 

    std::uint32_t index = 0;
    for (std::uint32_t i = 0; i < new_sample_count; i++)
    {
        index = i * mFmt.sample_rate / LOW_QUALITY_SAMPLE_RATE;
        raw_pcm.at(i) = (this->*getMonoSample)(width, raw_data, index * width * mFmt.num_channels);
    }
    
    return raw_pcm;
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
    ::memcpy(wav.mData.get(), raw_pcm, raw_pcm_size);
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

std::int16_t Wav::getMonoSample(std::uint32_t width, const void* data, std::uint32_t index) const
{
    std::int16_t temp_sample = 0;
    double collected_sample = 0;
    for (std::uint32_t k = 0; k < mFmt.num_channels; k++)
    {
        temp_sample = GETSAMPLE64(width, data, index + (width * k)) >> (64 - LOW_QUALITY_SAMPLE_WIDTH);
        collected_sample += temp_sample;
    }
    return std::int16_t(collected_sample / mFmt.num_channels);
}

std::int16_t Wav::monoToMonoSample(std::uint32_t width, const void* data, std::uint32_t index) const
{
    return GETSAMPLE64(width, data, index) >> (64 - LOW_QUALITY_SAMPLE_WIDTH);
}

std::int16_t Wav::stereoToMonoSample(std::uint32_t width, const void* data, std::uint32_t index) const
{
    std::int16_t sample1 = GETSAMPLE64(width, data, index) >> (64 - LOW_QUALITY_SAMPLE_WIDTH);
    std::int16_t sample2 = GETSAMPLE64(width, data, index + width) >> (64 - LOW_QUALITY_SAMPLE_WIDTH);
    return (sample1 + sample2) / 2;
}