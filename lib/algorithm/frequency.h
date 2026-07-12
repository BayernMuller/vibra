#ifndef LIB_ALGORITHM_FREQUENCY_H_
#define LIB_ALGORITHM_FREQUENCY_H_

#include <cstdint>

namespace vibra {

enum class FrequencyBand : std::int8_t {
  k0To150 = -1,
  k250To520,
  k520To1450,
  k1450To3500,
  k3500To5500,
};

struct FrequencyPeakData {
  std::uint32_t fft_pass_number;
  std::uint32_t peak_magnitude;
  std::uint32_t corrected_peak_frequency_bin;
  std::uint32_t sample_rate;
};

class FrequencyPeak {
 public:
  explicit FrequencyPeak(const FrequencyPeakData& data);
  ~FrequencyPeak() = default;

  inline std::uint32_t fft_pass_number() const { return fft_pass_number_; }
  inline std::uint32_t peak_magnitude() const { return peak_magnitude_; }
  inline std::uint32_t corrected_peak_frequency_bin() const {
    return corrected_peak_frequency_bin_;
  }
  double ComputeFrequency() const;
  double ComputeAmplitudePCM() const;
  double ComputeElapsedSeconds() const;

 private:
  std::uint32_t fft_pass_number_ = 0;
  std::uint32_t peak_magnitude_ = 0;
  std::uint32_t corrected_peak_frequency_bin_ = 0;
  std::uint32_t sample_rate_ = 0;
};

}  // namespace vibra

#endif  // LIB_ALGORITHM_FREQUENCY_H_
