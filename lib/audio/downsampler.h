#ifndef LIB_AUDIO_DOWNSAMPLER_H_
#define LIB_AUDIO_DOWNSAMPLER_H_

#include <cstdint>

#include <vector>

namespace vibra {

// forward declaration
class Wav;
//

using LowQualitySample = std::int16_t;
using LowQualityTrack = std::vector<LowQualitySample>;

constexpr std::uint32_t kLowQualitySampleRate = 16000;
constexpr std::uint32_t kLowQualitySampleBitWidth =
    sizeof(LowQualitySample) * 8;
constexpr std::uint32_t kLowQualitySampleMax = 32767;

struct DownsampleConfig {
  double ratio;
  std::uint32_t new_sample_count;
  std::uint32_t width;
  std::uint32_t channels;
};

using DownsampleFunc = void (*)(LowQualityTrack*, const void*,
                                const DownsampleConfig&);

class Downsampler {
 public:
  static LowQualityTrack GetLowQualityPCM(const Wav& wav,
                                          std::int32_t start_sec = 0,
                                          std::int32_t end_sec = -1);

 private:
  static DownsampleFunc getDownsampleFunc(bool is_signed, std::uint32_t width,
                                          std::uint32_t channels);

  static double readSignedFrameAsMono(const void* src, std::uint32_t frame,
                                      std::uint32_t width,
                                      std::uint32_t channels);

  template <typename T>
  static double readFloatFrameAsMono(const void* src, std::uint32_t frame,
                                     std::uint32_t width,
                                     std::uint32_t channels);

  static void signedStereoToMono(LowQualityTrack* dst, const void* src,
                                 const DownsampleConfig& config);
  static void signedMonoToMono(LowQualityTrack* dst, const void* src,
                               const DownsampleConfig& config);
  static void signedMultiToMono(LowQualityTrack* dst, const void* src,
                                const DownsampleConfig& config);

  template <typename T>
  static void floatStereoToMono(LowQualityTrack* dst, const void* src,
                                const DownsampleConfig& config);
  template <typename T>
  static void floatMonoToMono(LowQualityTrack* dst, const void* src,
                              const DownsampleConfig& config);
  template <typename T>
  static void floatMultiToMono(LowQualityTrack* dst, const void* src,
                               const DownsampleConfig& config);
};

}  // namespace vibra

#endif  // LIB_AUDIO_DOWNSAMPLER_H_
