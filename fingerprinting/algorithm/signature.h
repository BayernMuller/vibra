#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <map>
#include <list>
#include "frequency.h"

class Signature
{
public:
    Signature();
    ~Signature();

private:
    std::uint32_t mSampleRate;
    std::uint32_t mNumberOfSamples;
    std::map<FrequancyBand, std::list<FrequancyPeak>> mFrequancyBandToPeaks;
};

#endif // SIGNATURE_Hs
    
