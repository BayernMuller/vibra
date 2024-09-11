#include <emscripten/emscripten.h>
#include <iostream>
#include "../fingerprinting/algorithm/signature_generator.h"
#include "../fingerprinting/algorithm/signature.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DBG(x) std::cout << "[" << __FUNCTION__ << "] " << #x << ": " << x << std::endl;
struct SignatureWrapper
{
    std::string uri;
    unsigned int samplems;
};

SignatureWrapper* fingerprintWav(const Wav& wav)
{
    Raw16bitPCM pcm;
    wav.GetLowQualityPCM(&pcm);

    double duaration = pcm.size() / (double)LOW_QUALITY_SAMPLE_RATE;

    SignatureGenerator generator;
    generator.FeedInput(pcm);
    generator.SetMaxTimeSeconds(12);

    if (duaration > 12 * 3)
        generator.AddSampleProcessed(LOW_QUALITY_SAMPLE_RATE * ((int)duaration / 2) - 6);

    Signature signature = generator.GetNextSignature();
    
    static SignatureWrapper wrapper;
    wrapper.uri = signature.GetBase64Uri();
    wrapper.samplems = signature.NumberOfSamples() / signature.SampleRate() * 1000;
    return &wrapper;
}

SignatureWrapper* EMSCRIPTEN_KEEPALIVE GetWavSignature(char* raw_wav, int wav_data_size)
{
    Wav wav(raw_wav, wav_data_size);
    return fingerprintWav(wav);
}

SignatureWrapper* EMSCRIPTEN_KEEPALIVE GetPcmSignature(char* raw_pcm, int pcm_data_size, int sample_rate, int sample_width, int channel_count)
{
    Wav wav(raw_pcm, pcm_data_size, sample_rate, sample_width, channel_count);
    return fingerprintWav(wav);
}

char* EMSCRIPTEN_KEEPALIVE GetFingerprint(SignatureWrapper* signature)
{
    return const_cast<char*>(signature->uri.c_str());
}

unsigned int EMSCRIPTEN_KEEPALIVE GetSampleMs(SignatureWrapper* signature)
{
    return signature->samplems;
}

#ifdef __cplusplus
}
#endif

int main()
{
    return 0; // does nothing
}