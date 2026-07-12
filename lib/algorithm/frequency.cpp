#include "algorithm/frequency.h"

#include <cmath>

namespace vibra {

FrequencyPeak::FrequencyPeak(const FrequencyPeakData& data)
    : fft_pass_number_(data.fft_pass_number),
      peak_magnitude_(data.peak_magnitude),
      corrected_peak_frequency_bin_(data.corrected_peak_frequency_bin),
      sample_rate_(data.sample_rate) {}

double FrequencyPeak::ComputeFrequency() const {
  return corrected_peak_frequency_bin_ *
         (static_cast<double>(sample_rate_) / 2. / 1024. / 64.);
}

double FrequencyPeak::ComputeAmplitudePCM() const {
  return std::sqrt(std::exp((peak_magnitude_ - 6144) / 1477.3) * (1 << 17) /
                   2.) /
         1024.;
}

double FrequencyPeak::ComputeElapsedSeconds() const {
  return static_cast<double>(fft_pass_number_) * 128. /
         static_cast<double>(sample_rate_);
}

}  // namespace vibra
