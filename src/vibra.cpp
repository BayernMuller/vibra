#include "../include/vibra/vibra.h"
#include "communication/shazam.h"
#include "algorithm/signature_generator.h"

Fingerprint* _get_fingerprint(const Wav& wav);

Fingerprint* vibra_get_fingerprint_from_wav(const char* raw_wav, int wav_data_size)
{
    Wav wav(raw_wav, wav_data_size);
    return _get_fingerprint(wav);
}

Fingerprint* vibra_get_fingerprint_from_pcm(const char* raw_pcm, int pcm_data_size, int sample_rate, int sample_width, int channel_count)
{
    Wav wav(raw_pcm, pcm_data_size, sample_rate, sample_width, channel_count);
    return _get_fingerprint(wav);
}

const char* vibra_get_uri_from_fingerprint(Fingerprint* fingerprint)
{
    return fingerprint->uri.c_str();
}

unsigned int vibra_get_sample_ms_from_fingerprint(Fingerprint* fingerprint)
{
    return fingerprint->sample_ms;
}

const char* vibra_get_shazam_request_json(Fingerprint* fingerprint)
{
    static std::string content;
    content = Shazam::GetRequestContent(fingerprint->uri, fingerprint->sample_ms);
    return content.c_str();
}

Fingerprint* _get_fingerprint(const Wav& wav)
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
    
    static Fingerprint fingerprint;
    fingerprint.uri = Shazam::RequestMetadata(signature);
    fingerprint.sample_ms = signature.NumberOfSamples() * 1000 / signature.SampleRate();
    return &fingerprint;
}