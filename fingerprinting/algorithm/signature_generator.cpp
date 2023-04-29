#include "signature_generator.h"
#include "../utils/array.h"
#include "../utils/hanning.h"
#include "../utils/debug.h"

#include <algorithm>
#include <numeric>

SignatureGenerator::SignatureGenerator()
    : mInputPendingProcessing()
    , mSampleProcessed(0)
    , mNextSignature(16000, 0)
    , mRingBufferOfSamples(2048, 0)
    , mFFTOutputs(256, FFT::RealArray(1025, 0.0))
    , mSpreadFFTsOutput(256, FFT::RealArray(1025, 0.0))
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
        Raw16bitPCM input(mInputPendingProcessing.begin() + mSampleProcessed,
            mInputPendingProcessing.begin() + mSampleProcessed + 128);
        __PRINT__(input.size());
        
        mNextSignature.AddNumberOfSamples(128);
        
        doFFT(input);
        // doPeakSpreadingAndRecoginzation
        mSampleProcessed += 128;
        
        break; // TODO: remove this break
    }
    return mNextSignature;
}



void SignatureGenerator::doFFT(const Raw16bitPCM& input)
{
    std::copy(input.begin(), input.end(),
     mRingBufferOfSamples.begin() + mRingBufferOfSamples.Position());
    
    mRingBufferOfSamples.Position() += input.size();
    mRingBufferOfSamples.Position() %= 2048;
    mRingBufferOfSamples.NumWritten() += input.size();

    FFT::RealArray excerpt_from_ring_buffer;

    __PRINT__(mRingBufferOfSamples.Position());

    std::copy(mRingBufferOfSamples.begin() + mRingBufferOfSamples.Position(), mRingBufferOfSamples.end(),
        excerpt_from_ring_buffer.begin());

    __PRINT__("copying from the beginning");

    std::copy(mRingBufferOfSamples.begin(), mRingBufferOfSamples.begin() + mRingBufferOfSamples.Position(),
        excerpt_from_ring_buffer.end());

    array::multiply(excerpt_from_ring_buffer, HANNIG_MATRIX);

    FFT::RealArray real;
    FFT::RealArray imag;
    FFT::RFFT(excerpt_from_ring_buffer, real, imag);

    // do max((real^2 + imag^2) / (1 << 17), 0.0000000001)
    array::square(real);
    array::square(imag);
    
    auto fft_results = array::add(real, imag);

    array::devide(fft_results, 1 << 17);
    array::max(fft_results, 1e-10);

    mFFTOutputs.Append(fft_results);
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
    mFFTOutputs = RingBuffer<FFT::RealArray>(256, FFT::RealArray(1025, 0.0));
    mSpreadFFTsOutput = RingBuffer<FFT::RealArray>(256, FFT::RealArray(1025, 0.0));
}