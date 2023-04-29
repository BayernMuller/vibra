#include "signature.h"

Signature::Signature(std::uint32_t sampleRate, std::uint32_t numberOfSamples)
    : mSampleRate(sampleRate)
    , mNumberOfSamples(numberOfSamples)
{
}

void Signature::Reset(std::uint32_t sampleRate, std::uint32_t numberOfSamples)
{
    mSampleRate = sampleRate;
    mNumberOfSamples = numberOfSamples;
    mFrequancyBandToPeaks.clear();
}

std::uint32_t Signature::SumOfPeaksLength() const
{
    std::uint32_t sum = 0;
    for (const auto& pair : mFrequancyBandToPeaks)
    {
        sum += pair.second.size();
    }
    return sum;
}

Signature::~Signature()
{
}