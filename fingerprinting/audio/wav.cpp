#include "wav.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <memory.h>



Wav::Wav(const std::string& wav_file_path)
    : mWavFilePath(wav_file_path)
{
    readWavFile(wav_file_path);
}

Wav::~Wav()
{
}

void Wav::GetLowQualityPCM(Raw16bitPCM* raw_pcm)
{
    // clear raw_pcm
    raw_pcm->clear();

    const void* raw_data = mData.get();
    
    double downsample_ratio = mSampleRate / (double)LOW_QUALITY_SAMPLE_RATE;
    std::uint32_t width = mBitPerSample / 8;
    std::uint32_t sample_count = mDataSize / width;
    std::uint32_t new_sample_count = sample_count / mChannel / downsample_ratio;
    raw_pcm->resize(new_sample_count);

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
        raw_pcm->at(j) = (this->*getMonoSample)(width, raw_data, i);
    }
}

void Wav::readWavFile(const std::string& wav_file_path)
{
    std::ifstream wav_file(wav_file_path, std::ios::binary);
    assert(wav_file.is_open());

    // read whole file
    char* data = nullptr;
    wav_file.seekg(0, std::ios::end);
    std::uint64_t file_size = wav_file.tellg();
    mFileSize = file_size;

    data = new char[file_size];
    wav_file.seekg(0, std::ios::beg);
    wav_file.read(data, file_size);

    // Read RIFF 
    char* riff_header = data;
    assert(::strncmp(riff_header, "RIFF", 4) == 0);
    
    // Read WAVE
    char* wave_header = data + 8;
    assert(::strncmp(wave_header, "WAVE", 4) == 0);

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
            mData = std::shared_ptr<std::uint8_t>(new std::uint8_t[mDataSize]);
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

void Wav::SaveWavFile(const std::string& wav_file_path, Raw16bitPCM& raw_pcm, std::uint32_t sample_rate, std::uint32_t sample_width, std::uint32_t channel_count)
{
    std::ofstream wav_file(wav_file_path, std::ios::binary);
    assert(wav_file.is_open());

    std::uint32_t data_size = raw_pcm.size() * (sample_width / 8) * channel_count;
    
    wav_file.write("RIFF", 4);
    wav_file.write((char*)&data_size, 4);
    wav_file.write("WAVE", 4);
    wav_file.write("fmt ", 4);
    std::uint32_t fmt_size = 16;
    wav_file.write((char*)&fmt_size, 4);
    std::uint16_t audio_format = 1;
    wav_file.write((char*)&audio_format, 2);
    wav_file.write((char*)&channel_count, 2);
    wav_file.write((char*)&sample_rate, 4);
    std::uint32_t byte_rate = sample_rate * channel_count * (sample_width / 8);
    wav_file.write((char*)&byte_rate, 4);
    std::uint16_t block_align = channel_count * (sample_width / 8);
    wav_file.write((char*)&block_align, 2);
    wav_file.write((char*)&sample_width, 2);
    wav_file.write("data", 4);
    wav_file.write((char*)&data_size, 4);

    std::uint32_t width = sample_width / 8;
    for (std::uint32_t i = 0; i < raw_pcm.size(); i++)
    {
        u_int16_t val = raw_pcm[i];
        wav_file.write((char*)&val, width);
    }
}

std::uint32_t Wav::gcd(std::uint32_t a, std::uint32_t b)
{
    while (b != 0)
    {
        std::uint32_t temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

Sample Wav::getMonoSample(std::uint32_t width, const void* data, std::uint32_t index)
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

Sample Wav::monoToMonoSample(std::uint32_t width, const void* data, std::uint32_t index)
{
    return GETSAMPLE32(width, data, index) >> LOW_QUALITY_SAMPLE_WIDTH;
}

Sample Wav::stereoToMonoSample(std::uint32_t width, const void* data, std::uint32_t index)
{
    Sample sample1 = GETSAMPLE32(width, data, index) >> LOW_QUALITY_SAMPLE_WIDTH;
    Sample sample2 = GETSAMPLE32(width, data, index + width) >> LOW_QUALITY_SAMPLE_WIDTH;
    return (sample1 + sample2) / 2;
}