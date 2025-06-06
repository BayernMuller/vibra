#include <emscripten/emscripten.h>
#include <vibra.h>

#ifdef __cplusplus
extern "C"
{
#endif

Fingerprint *EMSCRIPTEN_KEEPALIVE GetWavSignature(char *raw_wav, int wav_data_size)
{
    return vibra_get_fingerprint_from_wav_data(raw_wav, wav_data_size);
}

Fingerprint *EMSCRIPTEN_KEEPALIVE GetSignedPcmSignature(char *raw_pcm, int pcm_data_size,
                                                        int sample_rate, int sample_width,
                                                        int channel_count)
{
    return vibra_get_fingerprint_from_signed_pcm(raw_pcm, pcm_data_size, sample_rate, sample_width,
                                                 channel_count);
}

Fingerprint *EMSCRIPTEN_KEEPALIVE GetFloatPcmSignature(char *raw_pcm, int pcm_data_size,
                                                       int sample_rate, int sample_width,
                                                       int channel_count)
{
    return vibra_get_fingerprint_from_float_pcm(raw_pcm, pcm_data_size, sample_rate, sample_width,
                                                channel_count);
}

const char *EMSCRIPTEN_KEEPALIVE GetFingerprint(Fingerprint *signature)
{
    return vibra_get_uri_from_fingerprint(signature);
}

unsigned int EMSCRIPTEN_KEEPALIVE GetSampleMs(Fingerprint *signature)
{
    return vibra_get_sample_ms_from_fingerprint(signature);
}

void EMSCRIPTEN_KEEPALIVE FreeFingerprint(Fingerprint *signature)
{
    vibra_free_fingerprint(signature);
}

#ifdef __cplusplus
}
#endif
