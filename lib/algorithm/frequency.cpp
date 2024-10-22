#include "algorithm/frequency.h"
#include <cmath>

FrequancyPeak::FrequancyPeak(std::uint32_t fft_pass_number, std::uint32_t peak_magnitude,
                             std::uint32_t corrected_peak_frequency_bin, std::uint32_t sample_rate)
    : mFFTPassNumber(fft_pass_number), mPeakMagnitude(peak_magnitude),
      mCorrectedPeakFrequencyBin(corrected_peak_frequency_bin), mSampleRate(sample_rate)
{
}

FrequancyPeak::~FrequancyPeak()
{
}

double FrequancyPeak::GetFrequencyHz() const
{
    return mCorrectedPeakFrequencyBin * (static_cast<double>(mSampleRate) / 2. / 1024. / 64.);
}

double FrequancyPeak::GetAmplitudePCM() const
{
    return std::sqrt(std::exp((mPeakMagnitude - 6144) / 1477.3) * (1 << 17) / 2.) / 1024.;
}

double FrequancyPeak::GetSeconds() const
{
    return static_cast<double>(mFFTPassNumber) * 128. / static_cast<double>(mSampleRate);
}
