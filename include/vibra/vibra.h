#ifndef __VIBRA_H__
#define __VIBRA_H__

#include <string>

extern "C" {

struct Fingerprint
{
    std::string uri;
    unsigned int sample_ms;
};

Fingerprint* vibra_get_fingerprint_from_wav(
    const char* raw_wav,
    int wav_data_size
);

Fingerprint* vibra_get_fingerprint_from_pcm(
    const char* raw_pcm,
    int pcm_data_size,
    int sample_rate,
    int sample_width,
    int channel_count
);

const char* vibra_get_uri_from_fingerprint(
    Fingerprint* fingerprint
);

unsigned int vibra_get_sample_ms_from_fingerprint(
    Fingerprint* fingerprint
);

const char* vibra_get_shazam_request_json(
    Fingerprint* fingerprint
);

} // extern "C"

#endif // __VIBRA_H__

