#include "wav.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <memory.h>
#include <sstream>  

Wav Wav::FromFile(const std::string& wav_file_path)
{
    Wav wav;
    wav.mWavFilePath = wav_file_path;
    wav.readWavFile(wav_file_path);
    return wav;
}

Wav Wav::FromRawWav(const char* raw_wav, std::uint32_t raw_wav_size)
{
    Wav wav;
    std::istringstream stream(std::string(raw_wav, raw_wav_size));
    wav.readWavBuffer(stream);
    return wav;
}

Wav Wav::FromRawPCM(const char* raw_pcm, std::uint32_t raw_pcm_size, std::uint32_t sample_rate, std::uint32_t sample_width, std::uint32_t channel_count)
{
    Wav wav;
    wav.mAudioFormat = 1;
    wav.mChannel = channel_count;
    wav.mSampleRate = sample_rate;
    wav.mBitPerSample = sample_width;
    wav.mDataSize = raw_pcm_size;
    wav.mFileSize = 44 + raw_pcm_size;
    wav.mData.reset(new std::uint8_t[raw_pcm_size]);
    ::memcpy(wav.mData.get(), raw_pcm, raw_pcm_size);
    return wav;
}

Wav::~Wav()
{
}

Raw16bitPCM Wav::GetLowQualityPCM(std::int32_t start_sec, std::int32_t end_sec) const
{
    Raw16bitPCM raw_pcm;

    if (mChannel == 1 && mSampleRate == LOW_QUALITY_SAMPLE_RATE && mBitPerSample == 16 && start_sec == 0 && end_sec == -1)
    {
        // no need to convert low quality pcm. just copy raw data
        raw_pcm.resize(mDataSize);
        ::memcpy(raw_pcm.data(), mData.get(), mDataSize);
        return raw_pcm;
    }
    
    
    double downsample_ratio = mSampleRate / (double)LOW_QUALITY_SAMPLE_RATE;
    std::uint32_t width = mBitPerSample / 8;
    std::uint32_t sample_count = mDataSize / width;

    const void* raw_data = mData.get() + (start_sec * mSampleRate * width * mChannel);

    std::uint32_t new_sample_count = sample_count / mChannel / downsample_ratio;

    if (end_sec != -1)
    {
        new_sample_count = (end_sec - start_sec) * LOW_QUALITY_SAMPLE_RATE;
    }

    raw_pcm.resize(new_sample_count);

    auto getMonoSample = &Wav::getMonoSample;
    if (mChannel == 1)
    {
        getMonoSample = &Wav::monoToMonoSample;
    }
    else if (mChannel == 2)
    {
        getMonoSample = &Wav::stereoToMonoSample;
    } 

    for (std::uint32_t i = 0, j = 0;
        i < mDataSize && j < new_sample_count;
        i += (width * mChannel) * downsample_ratio, j++)
    {
        raw_pcm.at(j) = (this->*getMonoSample)(width, raw_data, i);
    }
    return raw_pcm;
}

void Wav::readWavFile(const std::string& wav_file_path)
{
    std::ifstream wav_file(wav_file_path, std::ios::binary);
    assert(wav_file.is_open());
    readWavBuffer(wav_file);
}

void Wav::readWavBuffer(std::istream& stream)
{
    // read whole file
    char* data = nullptr;
    stream.seekg(0, std::ios::end);
    std::uint64_t file_size = stream.tellg();
    mFileSize = file_size;

    data = new char[file_size];
    stream.seekg(0, std::ios::beg);
    stream.read(data, file_size);

    // Read RIFF 
    char* riff_header = data;
    assert(::strncmp(riff_header, "RIFF", 4) == 0);
    (void)riff_header; // suppress warning
    
    // Read WAVE
    char* wave_header = data + 8;
    assert(::strncmp(wave_header, "WAVE", 4) == 0);
    (void)wave_header; // suppress warning

    // Read Audio Format
    mAudioFormat = *(std::uint16_t*)(data + 20);

    // Read Channel
    mChannel = *(std::uint16_t*)(data + 22);
    assert(mChannel == 1 || mChannel == 2); // only support mono and stereo now..

    // Read Sample Rate
    mSampleRate = *(std::uint32_t*)(data + 24);

    // Read Bit Per Sample
    mBitPerSample = *(std::uint16_t*)(data + 34);

    std::uint32_t pos = 12;
    std::uint32_t subchunks_limit = 10;
    while (pos + 8 <= file_size && subchunks_limit)
    {
        char* subchunk_id = data + pos;
        std::uint32_t subchunk_size = *(std::uint32_t*)(data + pos + 4);

        if (strncmp(subchunk_id, "data", 4) == 0)
        {
            mDataSize = subchunk_size;
            mData.reset(new std::uint8_t[mDataSize]);
            ::memcpy(mData.get(), data + pos + 8, mDataSize);
            
            delete[] data;

            return; // read data successfully
        }

        pos += subchunk_size + 8;
        --subchunks_limit;
    }
    delete[] data;
    assert(false); // read data failed   
}

Sample Wav::getMonoSample(std::uint32_t width, const void* data, std::uint32_t index) const
{
    Sample temp_sample = 0;
    double collected_sample = 0;
    for (std::uint32_t k = 0; k < mChannel; k++)
    {
        temp_sample = GETSAMPLE32(width, data, index + (width * k)) >> LOW_QUALITY_SAMPLE_WIDTH;
        collected_sample += temp_sample;
    }
    return Sample(collected_sample / mChannel);
}

Sample Wav::monoToMonoSample(std::uint32_t width, const void* data, std::uint32_t index) const
{
    return GETSAMPLE32(width, data, index) >> LOW_QUALITY_SAMPLE_WIDTH;
}

Sample Wav::stereoToMonoSample(std::uint32_t width, const void* data, std::uint32_t index) const
{
    Sample sample1 = GETSAMPLE32(width, data, index) >> LOW_QUALITY_SAMPLE_WIDTH;
    Sample sample2 = GETSAMPLE32(width, data, index + width) >> LOW_QUALITY_SAMPLE_WIDTH;
    return (sample1 + sample2) / 2;
}