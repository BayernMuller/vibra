#ifndef SIGNATURE_GENERATOR_H
#define SIGNATURE_GENERATOR_H


#include "signature.h"
#include "../utils/ring_buffer.h"
#include "../audio/downsampler.h"
#include "../utils/fft.h"

constexpr auto MAX_PEAKS = 255u;

class SignatureGenerator
{
public:
    SignatureGenerator();
    void FeedInput(const LowQualityTrack& input);
    Signature GetNextSignature();

    inline void AddSampleProcessed(std::uint32_t sampleProcessed)
    { mSampleProcessed += sampleProcessed;}

    inline void SetMaxTimeSeconds(double maxTimeSeconds)
    { mMaxTimeSeconds = maxTimeSeconds; }

private:
    void processInput(const LowQualityTrack& input);
    void doFFT(const LowQualityTrack& input);
    void doPeakSpreadingAndRecoginzation();
    void doPeakSpreading();
    void doPeakRecognition();
    void resetSignatureGenerater();

private:
    LowQualityTrack mInputPendingProcessing;
    std::uint32_t mSampleProcessed;
    double mMaxTimeSeconds;

    Signature mNextSignature;
    RingBuffer<std::int16_t> mRingBufferOfSamples;
    RingBuffer<fft::RealArray> mFFTOutputs;
    RingBuffer<fft::RealArray> mSpreadFFTsOutput;    
};

#endif // SIGNATURE_GENERATOR_H
