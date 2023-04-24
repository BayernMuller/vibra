#ifndef SIGNATURE_GENERATOR_H
#define SIGNATURE_GENERATOR_H

#include "ring_buffer.h"
#include "signature.h"
#include "../audio/wav.h"

class SignatureGenerator
{
public:
    void FeedInput(const Raw16bitPCM& input);
    Signature GetNextSignature();

private:
    void processInput();
    void doPeakSpreadingAndRecoginzation();
    void doPeakSpreading();

private:
    Raw16bitPCM mInputPendingProcessing;
    std::uint32_t mSampleProcessed;

    RingBuffer<std::uint32_t> mRingBufferOfSamples;
    RingBuffer<std::vector<double>> mFFTOutputs;
    RingBuffer<std::vector<double>> mSpreadFFTsOutput;    
};

#endif // SIGNATURE_GENERATOR_H
