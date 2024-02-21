#include <emscripten/emscripten.h>
#include <iostream>
#include "../fingerprinting/algorithm/signature_generator.h"
#include "../fingerprinting/algorithm/signature.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DBG(x) std::cout << "[" << __FUNCTION__ << "] " << #x << ": " << x << std::endl;

std::string gSignature;

char* EMSCRIPTEN_KEEPALIVE GetSignature(char* raw_wav, int wav_data_size)
{
    Wav wav(raw_wav, wav_data_size);

    Raw16bitPCM pcm;
    wav.GetLowQualityPCM(&pcm);

    double duaration = pcm.size() / (double)LOW_QUALITY_SAMPLE_RATE;

    SignatureGenerator generator;
    generator.FeedInput(pcm);
    generator.SetMaxTimeSeconds(12);
    generator.AddSampleProcessed(LOW_QUALITY_SAMPLE_RATE * ((int)duaration / 2) - 6);

    Signature signature = generator.GetNextSignature();
    gSignature = signature.GetBase64Uri();
    return const_cast<char*>(gSignature.c_str());
}

int main()
{
    return 0; // does nothing
}

#ifdef __cplusplus
}
#endif
