#ifndef FREQUENCY_H
#define FREQUENCY_H

#include <cstdint>

enum class FrequancyBand
{
    _0_150 = -1,
    _250_520,
    _520_1450,
    _1450_3500,
    _3500_5500,
};

class FrequancyPeak
{
public:
    FrequancyPeak(std::uint32_t fft_pass_number, std::uint32_t peak_magnitude, std::uint32_t corrected_peak_frequency_bin, std::uint32_t sample_rate);
    ~FrequancyPeak();

    double GetFrequencyHz() const;
    double GetAmplitudePCM() const;
    double GetSeconds() const;

private:
    std::uint32_t mFFTPassNumber;
    std::uint32_t mPeakMagnitude;
    std::uint32_t mCorrectedPeakFrequencyBin;
    std::uint32_t mSampleRate;
};

#endif // FREQUENCY_H