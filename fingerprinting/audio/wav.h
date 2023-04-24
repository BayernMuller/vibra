#ifndef WAV_H
#define WAV_H

#include <string>
#include <vector>

using Sample = std::int16_t;
using Raw16bitPCM = std::vector<Sample>;

class Wav
{
public:
    Wav(const std::string& wav_file_path);
    ~Wav();

    inline std::uint32_t GetAudioFormat() { return mAudioFormat; }
    inline std::uint32_t GetChannel() { return mChannel; }
    inline std::uint32_t GetSampleRate() { return mSampleRate; }
    inline std::uint32_t GetBitPerSample() { return mBitPerSample; }
    inline std::uint32_t GetDataSize() { return mDataSize; }
    inline std::uint32_t GetFileSize() { return mFileSize; }
    inline std::shared_ptr<std::uint8_t> GetData() const { return mData; }

    void GetLowQualityPCM(Raw16bitPCM& raw_pcm);
    void SaveWavFile(const std::string& wav_file_path, Raw16bitPCM& raw_pcm, std::uint32_t sample_rate, std::uint32_t sample_width, std::uint32_t channel_count);
private:
    void readWavFile(const std::string& wav_file_path);
    std::uint32_t gcd(std::uint32_t a, std::uint32_t b);
    

private:
    std::string mWavFilePath;
    std::uint32_t mAudioFormat;
    std::uint32_t mChannel;
    std::uint32_t mSampleRate;
    std::uint32_t mBitPerSample;
    std::uint32_t mDataSize;
    std::uint32_t mFileSize;
    std::shared_ptr<std::uint8_t> mData;
};

#endif // WAV_H