#ifndef SIGNATURE_GENERATOR_H
#define SIGNATURE_GENERATOR_H


#include "signature.h"
#include "../utils/ring_buffer.h"
#include "../audio/wav.h"
#include "../utils/fft.h"

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
    RingBuffer<FFT::RealArray> mFFTOutputs;
    RingBuffer<FFT::RealArray> mSpreadFFTsOutput;    
};

#endif // SIGNATURE_GENERATOR_H
