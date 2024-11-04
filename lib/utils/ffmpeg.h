#ifndef LIB_UTILS_FFMPEG_H_
#define LIB_UTILS_FFMPEG_H_

#include <cstdlib>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "audio/downsampler.h"

namespace ffmpeg
{

constexpr const char *DEFAULT_FFMPEG_PATHS[] = {"ffmpeg", "ffmpeg.exe"};
constexpr const char FFMPEG_PATH_ENV[] = "FFMPEG_PATH";

class FFmpegWrapper
{
public:
    FFmpegWrapper() = delete;
    static LowQualityTrack ConvertToLowQaulityPcm(
        std::string input_file, std::uint32_t start_seconds, std::uint32_t duration_seconds);

private:
    static std::string getFFmpegPath();
    static bool isWindows();
};

LowQualityTrack FFmpegWrapper::ConvertToLowQaulityPcm(
    std::string input_file, std::uint32_t start_seconds, std::uint32_t duration_seconds)
{
    static std::string ffmpeg_path = FFmpegWrapper::getFFmpegPath();
    if (ffmpeg_path.empty())
    {
        std::cerr << "FFmpeg not found on system. Please install FFmpeg or set the ";
        std::cerr << FFMPEG_PATH_ENV << " environment variable." << std::endl;
        throw std::runtime_error("FFmpeg not found");
    }

    std::stringstream ss;
    ss << ffmpeg_path;
    ss << " -i " << input_file;
    ss << " -f "
       << "s" << LOW_QUALITY_SAMPLE_BIT_WIDTH << "le";
    ss << " -acodec "
       << "pcm_s" << LOW_QUALITY_SAMPLE_BIT_WIDTH << "le";
    ss << " -ar " << LOW_QUALITY_SAMPLE_RATE;
    ss << " -ac " << 1;
    ss << " -ss " << start_seconds;
    ss << " -t " << duration_seconds;
    ss << " -"; // stdout
    ss << " 2>/dev/null"; // suppress std

    std::FILE *pipe = popen(ss.str().c_str(), "r");
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }

    std::array<std::int16_t, 4096> buffer;
    size_t bytes_read;

    LowQualityTrack pcm;
    pcm.reserve(duration_seconds * LOW_QUALITY_SAMPLE_RATE);

    while ((bytes_read = fread(buffer.data(), 1, buffer.size(), pipe)) != 0)
    {
        pcm.insert(pcm.end(), buffer.begin(),
                   buffer.begin() + (bytes_read / sizeof(LowQualitySample)));
    }

    int exit_code = pclose(pipe);
    if (exit_code != 0)
    {
        throw std::runtime_error("PCM Conversion Failed: " + std::to_string(exit_code));
    }
    return pcm;
}

std::string FFmpegWrapper::getFFmpegPath()
{
    const char *ffmpeg_env = std::getenv(FFMPEG_PATH_ENV);
    if (ffmpeg_env)
    {
        return ffmpeg_env;
    }

    std::string path = std::getenv("PATH");
    std::istringstream ss(path);
    std::string token;
    char delimiter = isWindows() ? ';' : ':';
    while (std::getline(ss, token, delimiter))
    {
        for (const char *ffmpeg_path : DEFAULT_FFMPEG_PATHS)
        {
            std::string full_path = token + "/" + ffmpeg_path;
            if (std::ifstream(full_path).good())
            {
                return full_path;
            }
        }
    }
    return ""; // empty string means FFmpeg not found
}

bool FFmpegWrapper::isWindows()
{
#if defined(_WIN32) || defined(_WIN64)
    return true;
#endif // _WIN32

    return false;
}

} // namespace ffmpeg

#endif // LIB_UTILS_FFMPEG_H_
