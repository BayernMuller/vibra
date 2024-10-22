#ifndef LIB_ALGORITHM_SIGNATURE_GENERATOR_H_
#define LIB_ALGORITHM_SIGNATURE_GENERATOR_H_

#include "algorithm/signature.h"
#include "audio/downsampler.h"
#include "utils/fft.h"
#include "utils/ring_buffer.h"

constexpr auto MAX_PEAKS = 255u;

class SignatureGenerator
{
  public:
    SignatureGenerator();
    void FeedInput(const LowQualityTrack &input);
    Signature GetNextSignature();

    inline void AddSampleProcessed(std::uint32_t sampleProcessed)
    {
        mSampleProcessed += sampleProcessed;
    }

    inline void SetMaxTimeSeconds(double maxTimeSeconds)
    {
        mMaxTimeSeconds = maxTimeSeconds;
    }

  private:
    void processInput(const LowQualityTrack &input);
    void doFFT(const LowQualityTrack &input);
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

#endif // LIB_ALGORITHM_SIGNATURE_GENERATOR_H_
