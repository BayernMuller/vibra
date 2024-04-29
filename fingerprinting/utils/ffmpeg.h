#ifndef __FFMPEG_H__
#define __FFMPEG_H__

#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../audio/wav.h"

namespace ffmpeg
{
    constexpr char DEFAULT_FFMPEG_PATH[] = "/opt/homebrew/bin/ffmpeg";
    constexpr char FFMPEG_PATH_ENV[] = "FFMPEG_PATH";
    constexpr int EXPECTED_DURATION = 5 * 60; // 5 minutes

    int convertToWav(const std::string &input_file, Raw16bitPCM* pcm)
    {
        std::string ffmpeg_path = DEFAULT_FFMPEG_PATH;
        const char *env = std::getenv(FFMPEG_PATH_ENV);
        if (env != nullptr)
        {
            ffmpeg_path = env;
        }

        if (std::ifstream(ffmpeg_path).good() == false)
        {
            throw std::runtime_error("ffmpeg not found");
        }

        std::stringstream ss;
        ss << ffmpeg_path;
        ss << " -i "        << input_file;
        ss << " -f "        << "s" << LOW_QUALITY_SAMPLE_WIDTH << "le";
        ss << " -acodec "   << "pcm_s" << LOW_QUALITY_SAMPLE_WIDTH << "le";
        ss << " -ar "       << LOW_QUALITY_SAMPLE_RATE;
        ss << " -ac "       << 1;
        ss << " - 2>/dev/null"; // suppress std

        std::FILE *pipe = popen(ss.str().c_str(), "r");
        if (!pipe)
        {
            throw std::runtime_error("popen() failed!");
        }

        std::array<Sample, 4096> buffer;
        size_t bytes_read;

        pcm->reserve(EXPECTED_DURATION * LOW_QUALITY_SAMPLE_RATE);

        while ((bytes_read = fread(buffer.data(), 1, buffer.size(), pipe)) != 0) 
        {
            pcm->insert(pcm->end(), buffer.begin(), buffer.begin() + (bytes_read / sizeof(Sample)));
        }

        fclose(pipe);
        return 0;
    }
}

#endif // __FFMPEG_H__
