#include "downsampler.h"
#include "byte_control.h"
#include "wav.h"

LowQualityTrack Downsampler::GetLowQualityPCM(const Wav& wav, std::int32_t start_sec, std::int32_t end_sec)
{
    LowQualityTrack low_quality_pcm;

    const auto channels = wav.GetChannel();
    const auto sample_rate = wav.GetSampleRate();
    const auto bits_per_sample = wav.GetBitPerSample();
    const auto data_size = wav.GetDataSize();
    const auto audio_format = wav.GetAudioFormat();
    const std::uint8_t* pcm_data = wav.GetData().get();

    if (channels == 1 && 
        sample_rate == LOW_QUALITY_SAMPLE_RATE && 
        bits_per_sample == LOW_QUALITY_SAMPLE_BIT_WIDTH && 
        start_sec == 0 && 
        end_sec == -1)
    {
        // no need to convert low quality pcm. just copy raw data
        low_quality_pcm.resize(data_size);
        ::memcpy(low_quality_pcm.data(), wav.GetData().get(), data_size);
        return low_quality_pcm;
    }
    
    
    double downsample_ratio = sample_rate / (double)LOW_QUALITY_SAMPLE_RATE;
    std::uint32_t width = bits_per_sample / 8;
    std::uint32_t sample_count = data_size / width;

    const void* src_raw_data = pcm_data + (start_sec * sample_rate * width * channels);

    std::uint32_t new_sample_count = sample_count / channels / downsample_ratio;

    if (end_sec != -1)
    {
        new_sample_count = (end_sec - start_sec) * LOW_QUALITY_SAMPLE_RATE;
    }

    low_quality_pcm.resize(new_sample_count);

    auto downsample_func = &Downsampler::signedMonoToMono;
    bool is_signed = audio_format == 1;

    if (channels == 1)
    {
        if (is_signed)
        {
            downsample_func = &Downsampler::signedMonoToMono;
        }
        else
        {
            if (bits_per_sample == 32)
            {
                downsample_func = &Downsampler::floatMonoToMono<float>;
            }
            else if (bits_per_sample == 64)
            {
                downsample_func = &Downsampler::floatMonoToMono<double>;
            }
        }
    }
    else if (channels == 2)
    {
        if (is_signed)
        {
            downsample_func = &Downsampler::signedStereoToMono;
        }
        else
        {
            if (bits_per_sample == 32)
            {
                downsample_func = &Downsampler::floatStereoToMono<float>;
            }
            else if (bits_per_sample == 64)
            {
                downsample_func = &Downsampler::floatStereoToMono<double>;
            }
        }
    }
    else
    {
        if (is_signed)
        {
            downsample_func = &Downsampler::signedMultiToMono;
        }
        else
        {
            if (bits_per_sample == 32)
            {
                downsample_func = &Downsampler::floatMultiToMono<float>;
            }
            else if (bits_per_sample == 64)
            {
                downsample_func = &Downsampler::floatMultiToMono<double>;
            }
        }
    }

    downsample_func(
        &low_quality_pcm,
        src_raw_data,
        downsample_ratio,
        new_sample_count,
        width,
        channels
    );
    
    return low_quality_pcm;
}

void Downsampler::signedMonoToMono(
    LowQualityTrack* dst,
    const void* src,
    double downsample_ratio,
    std::uint32_t new_sample_count,
    std::uint32_t width,
    std::uint32_t channels)
{
    std::uint32_t index = 0;
    for (std::uint32_t i = 0; i < new_sample_count; i++)
    {
        index = std::uint32_t(i * downsample_ratio) * width * channels;
        dst->at(i) = GETSAMPLE64(width, src, index) >> (64 - LOW_QUALITY_SAMPLE_BIT_WIDTH);
    }
}

void Downsampler::signedStereoToMono(
    LowQualityTrack* dst,
    const void* src,
    double downsample_ratio,
    std::uint32_t new_sample_count,
    std::uint32_t width,
    std::uint32_t channels)
{
    std::uint32_t index = 0;
    for (std::uint32_t i = 0; i < new_sample_count; i++)
    {
        index = std::uint32_t(i * downsample_ratio) * width * channels;
        LowQualitySample sample1 = GETSAMPLE64(width, src, index) >> (64 - LOW_QUALITY_SAMPLE_BIT_WIDTH);
        LowQualitySample sample2 = GETSAMPLE64(width, src, index + width) >> (64 - LOW_QUALITY_SAMPLE_BIT_WIDTH);
        dst->at(i) = (sample1 + sample2) / 2;
    }
}

void Downsampler::signedMultiToMono(
    LowQualityTrack* dst,
    const void* src,
    double downsample_ratio,
    std::uint32_t new_sample_count,
    std::uint32_t width,
    std::uint32_t channels)
{
    double collected_sample = 0;
    std::uint32_t index = 0;
    for (std::uint32_t i = 0; i < new_sample_count; i++)
    {
        collected_sample = 0;
        for (std::uint32_t k = 0; k < channels; k++)
        {
            index = std::uint32_t(i * downsample_ratio) * width * channels;
            collected_sample += GETSAMPLE64(width, src, index + k * width) >> (64 - LOW_QUALITY_SAMPLE_BIT_WIDTH);
        }
        dst->at(i) = LowQualitySample(collected_sample / channels);
    }
}

template <typename T>
void Downsampler::floatMonoToMono(
    LowQualityTrack* dst,
    const void* src,
    double downsample_ratio,
    std::uint32_t new_sample_count,
    std::uint32_t width,
    std::uint32_t channels)
{
    T temp_float_sample = 0;
    std::uint64_t temp_sample = 0;
    std::uint32_t index = 0;
    for (std::uint32_t i = 0; i < new_sample_count; i++)
    {
        index = std::uint32_t(i * downsample_ratio) * width * channels;
        temp_sample = GETSAMPLE64(width, src, index) >> (64 - sizeof(T) * 8);
        temp_float_sample = *reinterpret_cast<T*>(&temp_sample);
        dst->at(i) = LowQualitySample(temp_float_sample * LOW_QUALITY_SAMPLE_MAX);
    }
}

template <typename T>
void Downsampler::floatStereoToMono(
    LowQualityTrack* dst,
    const void* src,
    double downsample_ratio,
    std::uint32_t new_sample_count,
    std::uint32_t width,
    std::uint32_t channels)
{
    std::uint64_t temp_sample1 = 0;
    std::uint64_t temp_sample2 = 0;
    T temp_float_sample1 = 0;
    T temp_float_sample2 = 0;
    std::uint32_t index = 0;
    for (std::uint32_t i = 0; i < new_sample_count; i++)
    {
        index = std::uint32_t(i * downsample_ratio) * width * channels;
        temp_sample1 = GETSAMPLE64(width, src, index) >> (64 - sizeof(T) * 8);
        temp_sample2 = GETSAMPLE64(width, src, index + width) >> (64 - sizeof(T) * 8);
        temp_float_sample1 = *reinterpret_cast<T*>(&temp_sample1);
        temp_float_sample2 = *reinterpret_cast<T*>(&temp_sample2);
        dst->at(i) = LowQualitySample((temp_float_sample1 + temp_float_sample2) / 2 * LOW_QUALITY_SAMPLE_MAX);
    }
}

template <typename T>
void Downsampler::floatMultiToMono(
    LowQualityTrack* dst,
    const void* src,
    double downsample_ratio,
    std::uint32_t new_sample_count,
    std::uint32_t width,
    std::uint32_t channels)
{
    std::uint32_t index = 0;
    T collected_sample = 0;
    std::uint64_t temp_sample = 0;
    for (std::uint32_t i = 0; i < new_sample_count; i++)
    {
        collected_sample = 0;
        index = std::uint32_t(i * downsample_ratio) * width * channels;
        for (std::uint32_t k = 0; k < channels; k++)
        {
            temp_sample = GETSAMPLE64(width, src, index + k * width) >> (64 - sizeof(T) * 8);
            collected_sample += *reinterpret_cast<T*>(&temp_sample);
        }
        dst->at(i) = LowQualitySample(collected_sample / channels * LOW_QUALITY_SAMPLE_MAX);
    }
}
