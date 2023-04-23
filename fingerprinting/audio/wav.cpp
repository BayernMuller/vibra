#include "wav.h"
#include <fstream>
#include <iostream>
#include <string>

#define GETINTX(T, cp, i)  (*(T *)((unsigned char *)(cp) + (i)))
#define SETINTX(T, cp, i, val)  do {                    \
        *(T *)((unsigned char *)(cp) + (i)) = (T)(val); \
    } while (0)


#define GETINT8(cp, i)          GETINTX(signed char, (cp), (i))
#define GETINT16(cp, i)         GETINTX(int16_t, (cp), (i))
#define GETINT32(cp, i)         GETINTX(int32_t, (cp), (i))

#ifdef WORDS_BIGENDIAN
#define GETINT24(cp, i)  (                              \
        ((unsigned char *)(cp) + (i))[2] +              \
        (((unsigned char *)(cp) + (i))[1] * (1 << 8)) + \
        (((signed char *)(cp) + (i))[0] * (1 << 16)) )
#else
#define GETINT24(cp, i)  (                              \
        ((unsigned char *)(cp) + (i))[0] +              \
        (((unsigned char *)(cp) + (i))[1] * (1 << 8)) + \
        (((signed char *)(cp) + (i))[2] * (1 << 16)) )
#endif


#define SETINT8(cp, i, val)     SETINTX(signed char, (cp), (i), (val))
#define SETINT16(cp, i, val)    SETINTX(int16_t, (cp), (i), (val))
#define SETINT32(cp, i, val)    SETINTX(int32_t, (cp), (i), (val))

#ifdef WORDS_BIGENDIAN
#define SETINT24(cp, i, val)  do {                              \
        ((unsigned char *)(cp) + (i))[2] = (int)(val);          \
        ((unsigned char *)(cp) + (i))[1] = (int)(val) >> 8;     \
        ((signed char *)(cp) + (i))[0] = (int)(val) >> 16;      \
    } while (0)
#else
#define SETINT24(cp, i, val)  do {                              \
        ((unsigned char *)(cp) + (i))[0] = (int)(val);          \
        ((unsigned char *)(cp) + (i))[1] = (int)(val) >> 8;     \
        ((signed char *)(cp) + (i))[2] = (int)(val) >> 16;      \
    } while (0)
#endif


#define GETRAWSAMPLE(size, cp, i)  (                    \
        (size == 1) ? (int)GETINT8((cp), (i)) :         \
        (size == 2) ? (int)GETINT16((cp), (i)) :        \
        (size == 3) ? (int)GETINT24((cp), (i)) :        \
                      (int)GETINT32((cp), (i)))

#define SETRAWSAMPLE(size, cp, i, val)  do {    \
        if (size == 1)                          \
            SETINT8((cp), (i), (val));          \
        else if (size == 2)                     \
            SETINT16((cp), (i), (val));         \
        else if (size == 3)                     \
            SETINT24((cp), (i), (val));         \
        else                                    \
            SETINT32((cp), (i), (val));         \
    } while(0)


#define GETSAMPLE32(size, cp, i)  (                           \
        (size == 1) ? (int)GETINT8((cp), (i)) * (1 << 24) :   \
        (size == 2) ? (int)GETINT16((cp), (i)) * (1 << 16) :  \
        (size == 3) ? (int)GETINT24((cp), (i)) * (1 << 8) :   \
                      (int)GETINT32((cp), (i)))

#define SETSAMPLE32(size, cp, i, val)  do {     \
        if (size == 1)                          \
            SETINT8((cp), (i), (val) >> 24);    \
        else if (size == 2)                     \
            SETINT16((cp), (i), (val) >> 16);   \
        else if (size == 3)                     \
            SETINT24((cp), (i), (val) >> 8);    \
        else                                    \
            SETINT32((cp), (i), (val));         \
    } while(0)

Wav::Wav(const std::string& wav_file_path)
    : mWavFilePath(wav_file_path)
{
    readWavFile(wav_file_path);
}

Wav::~Wav()
{
}

void Wav::GetLowQualityPCM(Raw16bitPCM& raw_pcm)
{
    // clear raw_pcm
    raw_pcm.clear();
    
    std::uint32_t new_sample_width = 16;
    std::uint32_t new_sample_rate = 16000;

    void* raw_data = mData.get();
    std::uint32_t width = mBitPerSample / 8;
    std::uint32_t sample_count = mDataSize / width;
    std::uint32_t new_sample_count = sample_count / mChannel;
    
    raw_pcm.resize(new_sample_count);

    Sample sample = 0;
    double colleted_sample = 0;
    for (std::uint32_t i = 0, j = 0; i < mDataSize; i += width, j++)
    {
        Sample sample = (GETSAMPLE32(width, raw_data, i) >> new_sample_width);
        colleted_sample += sample;
        
        // if it's multi channel, collect sample and divide it by channel count (average)
        if ((j && j % mChannel == 0) || mChannel == 1) 
        {
            raw_pcm[j / mChannel] = colleted_sample / mChannel;
            colleted_sample = 0.0;
        }
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