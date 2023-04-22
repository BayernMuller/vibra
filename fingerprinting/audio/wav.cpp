#include "wav.h"
#include <fstream>
#include <iostream>
#include <string>

Wav::Wav(const std::string& wav_file_path)
    : mWavFilePath(wav_file_path)
{
    readWavFile(wav_file_path);
}

Wav::~Wav()
{
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
    assert(strncmp(riff_header, "RIFF", 4) == 0);
    
    // Read WAVE
    char* wave_header = data + 8;
    assert(strncmp(wave_header, "WAVE", 4) == 0);

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
            memcpy(mData.get(), data + pos + 8, mDataSize);
            
            delete[] data;

            return; // read data successfully
        }

        pos += subchunk_size + 8;
        --subchunks_limit;
    }
    delete[] data;
    assert(false); // read data failed
}
