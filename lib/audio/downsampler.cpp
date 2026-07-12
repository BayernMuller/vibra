#include "audio/downsampler.h"

#include <cmath>
#include <cstring>

#include <algorithm>
#include <map>
#include <tuple>

#include "audio/byte_control.h"
#include "audio/wav.h"

namespace vibra {

double Downsampler::readSignedFrameAsMono(const void* src, std::uint32_t frame,
                                          std::uint32_t width,
                                          std::uint32_t channels) {
  double sample = 0.0;
  for (std::uint32_t channel = 0; channel < channels; channel++) {
    const std::uint32_t kIndex = (frame * channels + channel) * width;
    sample +=
        GETSAMPLE64(width, src, kIndex) >> (64 - kLowQualitySampleBitWidth);
  }
  return sample / channels;
}

template <typename T>
double Downsampler::readFloatFrameAsMono(const void* src, std::uint32_t frame,
                                         std::uint32_t width,
                                         std::uint32_t channels) {
  double sample = 0.0;
  for (std::uint32_t channel = 0; channel < channels; channel++) {
    const std::uint32_t kIndex = (frame * channels + channel) * width;
    const std::uint64_t kRawSample =
        GETSAMPLE64(width, src, kIndex) >> (64 - sizeof(T) * 8);
    T float_sample = 0;
    std::memcpy(&float_sample, &kRawSample, sizeof(T));
    sample += float_sample * kLowQualitySampleMax;
  }
  return sample / channels;
}

LowQualityTrack Downsampler::GetLowQualityPCM(const Wav& wav,
                                              std::int32_t start_sec,
                                              std::int32_t end_sec) {
  LowQualityTrack low_quality_pcm;

  const auto kChannels = wav.num_channels();
  const auto kSampleRate = wav.sample_rate_();
  const auto kBitsPerSample = wav.bits_per_sample();
  const auto kDataSize = wav.data_size();
  const auto kAudioFormat = wav.audio_format();
  const std::uint8_t* pcm_data = wav.data();

  if (kChannels == 1 && kSampleRate == kLowQualitySampleRate &&
      kBitsPerSample == kLowQualitySampleBitWidth && start_sec == 0 &&
      end_sec == -1) {
    // no need to convert low quality pcm. just copy raw data
    low_quality_pcm.resize(kDataSize / sizeof(LowQualitySample));
    std::memcpy(low_quality_pcm.data(), wav.data(), kDataSize);
    return low_quality_pcm;
  }

  double downsample_ratio =
      kSampleRate / static_cast<double>(kLowQualitySampleRate);
  std::uint32_t width = kBitsPerSample / 8;
  std::uint32_t sample_count = kDataSize / width;

  const void* src_raw_data = pcm_data + (static_cast<std::size_t>(start_sec) *
                                         kSampleRate * width * kChannels);

  auto new_sample_count = static_cast<std::uint32_t>(
      static_cast<double>(sample_count) / kChannels / downsample_ratio);

  if (end_sec != -1) {
    new_sample_count =
        static_cast<std::uint32_t>(end_sec - start_sec) * kLowQualitySampleRate;
  }

  low_quality_pcm.resize(new_sample_count);

  auto downsample_func = &Downsampler::signedMonoToMono;
  bool is_signed = kAudioFormat == 1;

  downsample_func = getDownsampleFunc(is_signed, kBitsPerSample, kChannels);
  const DownsampleConfig kConfig = {downsample_ratio, new_sample_count, width,
                                    kChannels};
  downsample_func(&low_quality_pcm, src_raw_data, kConfig);
  return low_quality_pcm;
}

DownsampleFunc Downsampler::getDownsampleFunc(bool is_signed,
                                              std::uint32_t width,
                                              std::uint32_t channels) {
  channels = std::min(channels, 3u);
  width = is_signed ? 0 : width;

  static std::map<std::tuple<bool, std::uint32_t, std::uint32_t>,
                  DownsampleFunc>
      func_map{
          {std::make_tuple(true, 0, 1), &Downsampler::signedMonoToMono},
          {std::make_tuple(true, 0, 2), &Downsampler::signedMonoToMono},
          {std::make_tuple(true, 0, 3), &Downsampler::signedMonoToMono},
          {std::make_tuple(false, 32, 1), &Downsampler::floatMonoToMono<float>},
          {std::make_tuple(false, 32, 2), &Downsampler::floatMonoToMono<float>},
          {std::make_tuple(false, 32, 3), &Downsampler::floatMonoToMono<float>},
          {std::make_tuple(false, 64, 1),
           &Downsampler::floatMonoToMono<double>},
          {std::make_tuple(false, 64, 2),
           &Downsampler::floatMonoToMono<double>},
          {std::make_tuple(false, 64, 3),
           &Downsampler::floatMonoToMono<double>},
      };
  return func_map.at(std::make_tuple(is_signed, width, channels));
}

void Downsampler::signedMonoToMono(LowQualityTrack* dst, const void* src,
                                   const DownsampleConfig& config) {
  const std::uint32_t kSourceFrameCount =
      std::max(1u, static_cast<std::uint32_t>(
                       std::ceil(config.new_sample_count * config.ratio)));
  for (std::uint32_t i = 0; i < config.new_sample_count; i++) {
    const double kSourcePosition = i * config.ratio;
    auto frame = static_cast<std::uint32_t>(kSourcePosition);
    if (frame >= kSourceFrameCount) {
      frame = kSourceFrameCount - 1;
    }
    const std::uint32_t kNextFrame = std::min(frame + 1, kSourceFrameCount - 1);
    const double kFraction = kSourcePosition - frame;
    const double kCurrentSample =
        readSignedFrameAsMono(src, frame, config.width, config.channels);
    const double kNextSample =
        readSignedFrameAsMono(src, kNextFrame, config.width, config.channels);
    dst->at(i) = static_cast<LowQualitySample>(
        kCurrentSample * (1.0 - kFraction) + kNextSample * kFraction);
  }
}

void Downsampler::signedStereoToMono(LowQualityTrack* dst, const void* src,
                                     const DownsampleConfig& config) {
  signedMonoToMono(dst, src, config);
}

void Downsampler::signedMultiToMono(LowQualityTrack* dst, const void* src,
                                    const DownsampleConfig& config) {
  signedMonoToMono(dst, src, config);
}

template <typename T>
void Downsampler::floatMonoToMono(LowQualityTrack* dst, const void* src,
                                  const DownsampleConfig& config) {
  const std::uint32_t kSourceFrameCount =
      std::max(1u, static_cast<std::uint32_t>(
                       std::ceil(config.new_sample_count * config.ratio)));
  for (std::uint32_t i = 0; i < config.new_sample_count; i++) {
    const double kSourcePosition = i * config.ratio;
    auto frame = static_cast<std::uint32_t>(kSourcePosition);
    if (frame >= kSourceFrameCount) {
      frame = kSourceFrameCount - 1;
    }
    const std::uint32_t kNextFrame = std::min(frame + 1, kSourceFrameCount - 1);
    const double kFraction = kSourcePosition - frame;
    const double kCurrentSample =
        readFloatFrameAsMono<T>(src, frame, config.width, config.channels);
    const double kNextSample =
        readFloatFrameAsMono<T>(src, kNextFrame, config.width, config.channels);
    dst->at(i) = static_cast<LowQualitySample>(
        kCurrentSample * (1.0 - kFraction) + kNextSample * kFraction);
  }
}

template <typename T>
void Downsampler::floatStereoToMono(LowQualityTrack* dst, const void* src,
                                    const DownsampleConfig& config) {
  floatMonoToMono<T>(dst, src, config);
}

template <typename T>
void Downsampler::floatMultiToMono(LowQualityTrack* dst, const void* src,
                                   const DownsampleConfig& config) {
  floatMonoToMono<T>(dst, src, config);
}

}  // namespace vibra
