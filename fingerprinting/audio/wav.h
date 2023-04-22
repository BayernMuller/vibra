#include <string>
#include <vector>

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
    inline std::shared_ptr<std::uint8_t> GetData() { return mData; }

private:
    void readWavFile(const std::string& wav_file_path);

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