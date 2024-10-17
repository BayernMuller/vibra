#ifndef DOWNSAMPLER_H
#define DOWNSAMPLER_H

#include <cstdint>
#include <vector>

// forward declaration
class Wav;
//

using LowQualitySample = std::int16_t;
using LowQualityTrack = std::vector<LowQualitySample>;

constexpr std::uint32_t LOW_QUALITY_SAMPLE_RATE = 16000;
constexpr std::uint32_t LOW_QUALITY_SAMPLE_BIT_WIDTH = sizeof(LowQualitySample) * 8;
constexpr std::uint32_t LOW_QUALITY_SAMPLE_MAX = std::numeric_limits<LowQualitySample>::max();

class Downsampler
{
public:
    static LowQualityTrack GetLowQualityPCM(
        const Wav& wav,
        std::int32_t start_sec = 0,
        std::int32_t end_sec = -1
    );

private:
    static void signedStereoToMono(
        LowQualityTrack* dst,
        const void* src,
        std::uint32_t new_sample_count,
        std::uint32_t sample_rate,
        std::uint32_t width,
        std::uint32_t channels
    );
    static void signedMonoToMono(
        LowQualityTrack* dst,
        const void* src,
        std::uint32_t new_sample_count,
        std::uint32_t sample_rate,
        std::uint32_t width,
        std::uint32_t channels
    );
    static void signedMultiToMono(
        LowQualityTrack* dst,
        const void* src,
        std::uint32_t new_sample_count,
        std::uint32_t sample_rate,
        std::uint32_t width,
        std::uint32_t channels
    );
    static void floatStereoToMono(
        LowQualityTrack* dst,
        const void* src,
        std::uint32_t new_sample_count,
        std::uint32_t sample_rate,
        std::uint32_t width,
        std::uint32_t channels
    );
    static void floatMonoToMono(
        LowQualityTrack* dst,
        const void* src,
        std::uint32_t new_sample_count,
        std::uint32_t sample_rate,
        std::uint32_t width,
        std::uint32_t channels
    );
    static void floatMultiToMono(
        LowQualityTrack* dst,
        const void* src,
        std::uint32_t new_sample_count,
        std::uint32_t sample_rate,
        std::uint32_t width,
        std::uint32_t channels
    );
};

#endif // DOWNSAMPLER_H