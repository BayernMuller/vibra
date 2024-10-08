#ifndef __VIBRA_H__
#define __VIBRA_H__

#include <string>

extern "C" {

/**
 * @brief Structure to hold a music fingerprint.
 * 
 * @note The structure is thread-unsafe and does not require manual memory management 
 * for the returned pointer.
 */
struct Fingerprint
{
    std::string uri;        /**< The URI associated with the fingerprint. */
    unsigned int sample_ms; /**< The sample duration in milliseconds. */
};

/**
 * @brief Generate a fingerprint from a music file.
 * 
 * @param music_file_path The path to the music file.
 * @return Fingerprint* Pointer to the generated fingerprint.
 * 
 * @note This function is thread-unsafe and the returned pointer should not be freed.
 */
Fingerprint* vibra_get_fingerprint_from_music_file(
    const char* music_file_path
);

/**
 * @brief Generate a fingerprint from WAV data.
 * 
 * @param raw_wav The raw WAV data.
 * @param wav_data_size The size of the WAV data in bytes.
 * @return Fingerprint* Pointer to the generated fingerprint.
 * 
 * @note This function is thread-unsafe and the returned pointer should not be freed.
 */
Fingerprint* vibra_get_fingerprint_from_wav_data(
    const char* raw_wav,
    int wav_data_size
);

/**
 * @brief Generate a fingerprint from PCM data.
 * 
 * @param raw_pcm The raw PCM data.
 * @param pcm_data_size The size of the PCM data in bytes.
 * @param sample_rate The sample rate of the PCM data.
 * @param sample_width The sample width (bits per sample) of the PCM data.
 * @param channel_count The number of channels in the PCM data.
 * @return Fingerprint* Pointer to the generated fingerprint.
 * 
 * @note This function is thread-unsafe and the returned pointer should not be freed.
 */
Fingerprint* vibra_get_fingerprint_from_pcm(
    const char* raw_pcm,
    int pcm_data_size,
    int sample_rate,
    int sample_width,
    int channel_count
);

/**
 * @brief Generate a fingerprint from PCM data.
 * 
 * This function generates a fingerprint from raw PCM data. It processes the PCM
 * data according to the provided sample rate, sample width (bits per sample),
 * and channel count, returning a pointer to the generated fingerprint.
 * 
 * @param raw_pcm The raw PCM data buffer.
 * @param pcm_data_size The size of the PCM data in bytes.
 * @param sample_rate The sample rate of the PCM data (e.g., 44100 Hz).
 * @param sample_width The sample width of the PCM data, measured in bits per sample 
 *        (e.g., 16 for 16-bit PCM).
 * @param channel_count The number of audio channels in the PCM data (e.g., 2 for stereo).
 * @return Fingerprint* Pointer to the generated fingerprint. This function returns
 *         a raw pointer to the fingerprint, which should not be freed by the caller.
 * 
 * @note This function is thread-unsafe. The returned pointer should not be freed by the caller
 *       as it may be managed internally.
 * @note This function supports s16le, s24le, s32le, f32le, and f64le PCM data.
 */
Fingerprint* vibra_get_fingerprint_from_pcm(
    const char* raw_pcm,
    int pcm_data_size,
    int sample_rate,
    int sample_width,
    int channel_count
);

/**
 * @brief Get the URI associated with a fingerprint.
 * 
 * @param fingerprint Pointer to the fingerprint.
 * @return const char* The URI as a C-string.
 * 
 * @note This function is thread-unsafe and the returned pointer should not be freed.
 */
const char* vibra_get_uri_from_fingerprint(
    Fingerprint* fingerprint
);

/**
 * @brief Get the sample duration in milliseconds from a fingerprint.
 * 
 * @param fingerprint Pointer to the fingerprint.
 * @return unsigned int The sample duration in milliseconds.
 * 
 * @note This function is thread-unsafe.
 */
unsigned int vibra_get_sample_ms_from_fingerprint(
    Fingerprint* fingerprint
);

/**
 * @brief Generate a Shazam request JSON from a fingerprint.
 * 
 * @param fingerprint Pointer to the fingerprint.
 * @return const char* The Shazam request JSON as a C-string.
 * 
 * @note This function is thread-unsafe and the returned pointer should not be freed.
 */
const char* vibra_get_shazam_request_json(
    const Fingerprint* fingerprint
);

/**
 * @brief Get the Shazam host URL.
 * 
 * @return const char* The Shazam host URL as a C-string.
 * 
 * @note This function is thread-unsafe and the returned pointer should not be freed.
 */
const char* vibra_get_shazam_host();

/**
 * @brief Get a random Shazam user agent string.
 * 
 * @return const char* A random Shazam user agent as a C-string.
 * 
 * @note This function is thread-unsafe and the returned pointer should not be freed.
 */
const char* vibra_get_shazam_random_user_agent();

} // extern "C"

#endif // __VIBRA_H__