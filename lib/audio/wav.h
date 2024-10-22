#ifndef LIB_AUDIO_WAV_H_
#define LIB_AUDIO_WAV_H_

#include <string>
#include <memory>
#include "audio/byte_control.h"

struct WavHeader {
    char riff_header[4]; // "RIFF"
    std::uint32_t file_size;
    char wave_header[4]; // "WAVE"
};

struct FmtSubchunk {
    std::uint16_t audio_format; // 1 = PCM, 3 = IEEE float, etc.
    std::uint16_t num_channels;
    std::uint32_t sample_rate;
    std::uint32_t byte_rate;
    std::uint16_t block_align;
    std::uint16_t bits_per_sample;
};

enum class AudioFormat {
    PCM_INTEGER = 1,
    PCM_FLOAT = 3,
};


class Wav
{
public:
    Wav(Wav&&) = default;
    Wav(const Wav&) = delete;
    static Wav FromFile(const std::string& wav_file_path);
    static Wav FromRawWav(const char* raw_wav, std::uint32_t raw_wav_size);
    static Wav FromSignedPCM(const char* raw_pcm, std::uint32_t raw_pcm_size,
                        std::uint32_t sample_rate, std::uint32_t sample_width,
                        std::uint32_t channel_count);
    static Wav FromFloatPCM(const char* raw_pcm, std::uint32_t raw_pcm_size,
                        std::uint32_t sample_rate, std::uint32_t sample_width,
                        std::uint32_t channel_count);
    ~Wav();

    inline std::uint16_t GetAudioFormat() const { return mFmt.audio_format; }
    inline std::uint16_t GetChannel() const { return mFmt.num_channels; }
    inline std::uint32_t GetSampleRate() const { return mFmt.sample_rate; }
    inline std::uint32_t GetBitPerSample() const { return mFmt.bits_per_sample; }
    inline std::uint32_t GetDataSize() const { return mDataSize; }
    inline std::uint32_t GetFileSize() const { return mHeader.file_size; }
    inline const std::unique_ptr<std::uint8_t[]>& GetData() const { return mData; }

private:
    Wav() = default;
    static Wav fromPCM(const char* raw_pcm, std::uint32_t raw_pcm_size, AudioFormat audio_format,
                        std::uint32_t sample_rate, std::uint32_t sample_width,
                        std::uint32_t channel_count);
    void readWavFileBuffer(std::istream& stream);

private:
    WavHeader mHeader;
    FmtSubchunk mFmt;
    std::string mWavFilePath;
    std::uint32_t mDataSize;
    std::unique_ptr<std::uint8_t[]> mData;
};

#endif // LIB_AUDIO_WAV_H_
