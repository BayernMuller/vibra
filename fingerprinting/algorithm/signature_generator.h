#ifndef SIGNATURE_GENERATOR_H
#define SIGNATURE_GENERATOR_H

#include "ring_buffer.h"
#include "signature.h"
#include "../audio/wav.h"

constexpr auto MAX_TIME_SECONDS = 3.1;
constexpr auto MAX_PEAKS = 255u;

class SignatureGenerator
{
public:
    SignatureGenerator();
    void FeedInput(const Raw16bitPCM& input);
    Signature GetNextSignature();

private:
    void doFFT(const Raw16bitPCM& input);
    void doPeakSpreadingAndRecoginzation();
    void doPeakSpreading();
    void doPeakRecognition();
    void prepareInput();

private:
    Raw16bitPCM mInputPendingProcessing;
    std::uint32_t mSampleProcessed;

    Signature mNextSignature;
    RingBuffer<std::uint16_t> mRingBufferOfSamples;
    RingBuffer<std::vector<double>> mFFTOutputs;
    RingBuffer<std::vector<double>> mSpreadFFTsOutput;    
};

#endif // SIGNATURE_GENERATOR_H
