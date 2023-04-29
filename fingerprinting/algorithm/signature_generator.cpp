#include "signature_generator.h"
#include <algorithm>
#include <numeric>

SignatureGenerator::SignatureGenerator()
    : mInputPendingProcessing()
    , mSampleProcessed(0)
    , mNextSignature(16000, 0)
    , mRingBufferOfSamples(2048, 0)
    , mFFTOutputs(256, std::vector<double>(1025, 0.0))
    , mSpreadFFTsOutput(256, std::vector<double>(1025, 0.0))
{
}

void SignatureGenerator::FeedInput(const Raw16bitPCM& input)
{
    mInputPendingProcessing.reserve(mInputPendingProcessing.size() + input.size());
    mInputPendingProcessing.insert(mInputPendingProcessing.end(), input.begin(), input.end());
}

Signature SignatureGenerator::GetNextSignature()
{
    if (mInputPendingProcessing.size() - mSampleProcessed < 128)
    {
        // failed.
    }

    while (mInputPendingProcessing.size() - mSampleProcessed >= 128 &&
        mNextSignature.NumberOfSamples() / mNextSignature.SampleRate() < MAX_TIME_SECONDS ||
        mNextSignature.SumOfPeaksLength() < MAX_PEAKS)
    {
        mNextSignature.AddNumberOfSamples(128);
        // doFFT
        // doPeakSpreadingAndRecoginzation
        mSampleProcessed += 128;
    }

    
}



void SignatureGenerator::doFFT(const Raw16bitPCM& input)
{
    std::copy(input.begin(), input.end(),
     mRingBufferOfSamples.begin() + mRingBufferOfSamples.Position());
    
    mRingBufferOfSamples.Position() += input.size();
    mRingBufferOfSamples.Position() %= 2048;
    mRingBufferOfSamples.NumWritten() += input.size();

    Raw16bitPCM excerpt_from_ring_buffer;

    std::copy(mRingBufferOfSamples.begin() + mRingBufferOfSamples.Position(), mRingBufferOfSamples.end(),
        excerpt_from_ring_buffer.begin());

    std::copy(mRingBufferOfSamples.begin(), mRingBufferOfSamples.begin() + mRingBufferOfSamples.Position(),
        excerpt_from_ring_buffer.end());



}

void SignatureGenerator::doPeakSpreadingAndRecoginzation()
{
}

void SignatureGenerator::doPeakSpreading()
{
}

void SignatureGenerator::prepareInput()
{
    mNextSignature.Reset(16000, 0);
    mRingBufferOfSamples = RingBuffer<std::uint16_t>(2048, 0);
    mFFTOutputs = RingBuffer<std::vector<double>>(256, std::vector<double>(1025, 0.0));
    mSpreadFFTsOutput = RingBuffer<std::vector<double>>(256, std::vector<double>(1025, 0.0));
}