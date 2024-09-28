#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <map>
#include <list>
#include <memory>
#include <sstream>
#include "frequency.h"

// Prevent Structure Padding
#ifdef _MSC_VER
    #pragma pack(push, 1)
#endif
struct RawSignatureHeader {
    uint32_t magic1;
    uint32_t crc32;
    uint32_t size_minus_header;
    uint32_t magic2;
    uint32_t void1[3];
    uint32_t shifted_sample_rate_id;
    uint32_t void2[2];
    uint32_t number_samples_plus_divided_sample_rate;
    uint32_t fixed_value;
} 
#ifdef _MSC_VER
    #pragma pack(pop)
#else
    __attribute__((packed)); 
#endif

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
    std::string GetBase64Uri() const;

private:
    template <typename T>
    std::stringstream& writeLittleEndian(std::stringstream& stream, const T&& value, size_t size = sizeof(T)) const
    {
        for (size_t i = 0; i < size; ++i)
        {
            stream << static_cast<char>(value >> (i << 3));
        }
        return stream;
    }

private:
    std::uint32_t mSampleRate;
    std::uint32_t mNumberOfSamples;
    std::map<FrequancyBand, std::list<FrequancyPeak>> mFrequancyBandToPeaks;
};

#endif // SIGNATURE_Hs
    
