#ifndef WAV_H
#define WAV_H

#include <string>
#include <vector>
#include <memory>
#include "byte_control.h"

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

using Raw16bitPCM = std::vector<std::int16_t>;

constexpr std::uint32_t LOW_QUALITY_SAMPLE_RATE = 16000;
constexpr std::uint32_t LOW_QUALITY_SAMPLE_WIDTH = sizeof(Raw16bitPCM::value_type) * 8;
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

    inline std::uint16_t GetAudioFormat() { return mFmt.audio_format; }
    inline std::uint16_t GetChannel() { return mFmt.num_channels; }
    inline std::uint32_t GetSampleRate() { return mFmt.sample_rate; }
    inline std::uint32_t GetBitPerSample() { return mFmt.bits_per_sample; }
    inline std::uint32_t GetDataSize() { return mDataSize; }
    inline std::uint32_t GetFileSize() { return mHeader.file_size; }
    Raw16bitPCM GetLowQualityPCM(std::int32_t start_sec = 0, std::int32_t end_sec = -1) const;

private:
    Wav() = default;
    static Wav fromPCM(const char* raw_pcm, std::uint32_t raw_pcm_size, AudioFormat audio_format,
                        std::uint32_t sample_rate, std::uint32_t sample_width,
                        std::uint32_t channel_count);
    void readWavFileBuffer(std::istream& stream);

    inline std::int16_t stereoToMonoSample(std::uint32_t width, const void* data, std::uint32_t index) const;
    inline std::int16_t monoToMonoSample(std::uint32_t width, const void* data, std::uint32_t index) const;
    inline std::int16_t getMonoSample(std::uint32_t width, const void* data, std::uint32_t index) const;


private:
    WavHeader mHeader;
    FmtSubchunk mFmt;
    std::string mWavFilePath;
    std::uint32_t mDataSize;
    std::unique_ptr<std::uint8_t[]> mData;
};

#endif // WAV_H