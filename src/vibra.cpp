#include "../include/vibra.h"
#include "communication/shazam.h"
#include "algorithm/signature_generator.h"
#include "utils/ffmpeg.h"

Fingerprint* _get_fingerprint_from_wav(const Wav& wav);

Fingerprint* _get_fingerprint_from_low_quality_pcm(const Raw16bitPCM& pcm);

Fingerprint* vibra_get_fingerprint_from_music_file(const char* music_file_path)
{
    std::string path = music_file_path;
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".wav")
    {
        Wav wav(music_file_path);
        return _get_fingerprint_from_wav(wav);
    }

    Raw16bitPCM pcm;
    ffmpeg::FFmpegWrapper::convertToWav(path, &pcm);
    return _get_fingerprint_from_low_quality_pcm(pcm);
}

Fingerprint* vibra_get_fingerprint_from_wav_data(const char* raw_wav, int wav_data_size)
{
    Wav wav(raw_wav, wav_data_size);
    return _get_fingerprint_from_wav(wav);
}

Fingerprint* vibra_get_fingerprint_from_pcm(const char* raw_pcm, int pcm_data_size, int sample_rate, int sample_width, int channel_count)
{
    Wav wav(raw_pcm, pcm_data_size, sample_rate, sample_width, channel_count);
    return _get_fingerprint_from_wav(wav);
}

const char* vibra_get_uri_from_fingerprint(Fingerprint* fingerprint)
{
    return fingerprint->uri.c_str();
}

unsigned int vibra_get_sample_ms_from_fingerprint(Fingerprint* fingerprint)
{
    return fingerprint->sample_ms;
}

const char* vibra_get_shazam_request_json(const Fingerprint* fingerprint)
{
    static std::string content;
    content = Shazam::GetRequestContent(fingerprint->uri, fingerprint->sample_ms);
    return content.c_str();
}

const char* vibra_get_shazam_host()
{
    static std::string host = Shazam::GetShazamHost();
    return host.c_str();
}

const char* vibra_get_shazam_random_user_agent()
{
    static std::string user_agent = Shazam::GetUserAgent();
    return user_agent.c_str();
}

Fingerprint* _get_fingerprint_from_wav(const Wav& wav)
{
    Raw16bitPCM pcm;
    wav.GetLowQualityPCM(&pcm);
    return _get_fingerprint_from_low_quality_pcm(pcm);
}

Fingerprint* _get_fingerprint_from_low_quality_pcm(const Raw16bitPCM& pcm)
{
    SignatureGenerator generator;
    generator.FeedInput(pcm);
    generator.SetMaxTimeSeconds(12);

    Signature signature = generator.GetNextSignature();
    
    static Fingerprint fingerprint;
    fingerprint.uri = signature.GetBase64Uri();
    fingerprint.sample_ms = signature.NumberOfSamples() * 1000 / signature.SampleRate();
    return &fingerprint;
}