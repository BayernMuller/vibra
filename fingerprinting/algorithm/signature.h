#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <map>
#include <list>
#include "frequency.h"

class Signature
{
public:
    Signature(std::uint32_t sampleRate, std::uint32_t numberOfSamples);
    ~Signature();
    void Reset(std::uint32_t sampleRate, std::uint32_t numberOfSamples);

    inline void AddNumberOfSamples(std::uint32_t numberOfSamples) { mNumberOfSamples += numberOfSamples; }
    inline std::uint32_t SampleRate() const { return mSampleRate; }
    inline std::uint32_t NumberOfSamples() const { return mNumberOfSamples; }
    inline std::map<FrequancyBand, std::list<FrequancyPeak>>& FrequancyBandToPeaks() { return mFrequancyBandToPeaks; }
    std::uint32_t SumOfPeaksLength() const;

private:
    std::uint32_t mSampleRate;
    std::uint32_t mNumberOfSamples;
    std::map<FrequancyBand, std::list<FrequancyPeak>> mFrequancyBandToPeaks;
};

#endif // SIGNATURE_Hs
    
