#ifndef __CLI_H__
#define __CLI_H__

#include <string>
#include <map>
#include <functional>
#include "../fingerprinting/algorithm/signature.h"

struct Action
{
    std::string description;
    std::string param;
    std::function<std::string(std::string)> func;
};

class CLIMain 
{
public:
    CLIMain(int argc, char** argv);
    ~CLIMain();
    void Run();

private:
    std::string fingerprintFromWavFile(std::string filepath);
    std::string recognizeSongFromWavFile(std::string filepath);

    std::string fingerprintFromRawPCM(std::string chunk_seconds);
    std::string recognizeSongFromRawPCM(std::string chunk_seconds);
    void help();

private:
    Signature getSignatureFromWavFile(std::string filepath);
    Signature getSignatureFromRawPCM(int chunk_seconds);

private:
    std::string m_action;
    std::string m_param;
    std::map<std::string, Action> m_commands{
        {"fingerprint-from-wav-file", 
            {
                "Generate fingerprint from wav file",
                "wav_file_path",
                std::bind(&CLIMain::fingerprintFromWavFile, this, std::placeholders::_1)
            }
        },
        {"recognize-song-from-wav-file", 
            {
                "Recognize song from wav file",
                "wav_file_path",
                std::bind(&CLIMain::recognizeSongFromWavFile, this, std::placeholders::_1)
            }
        },
        {"fingerprint-from-raw-pcm", 
            {
                "Generate fingerprint from raw PCM (s16le, mono, 16kHz) via stdin.",
                "audio_chunk_seconds",
                std::bind(&CLIMain::fingerprintFromRawPCM, this, std::placeholders::_1)
            }
        },
        {"recognize-song-from-raw-pcm", 
            {
                "Recognize song from raw PCM (s16le, mono, 16kHz) via stdin.",
                "audio_chunk_seconds",
                std::bind(&CLIMain::recognizeSongFromRawPCM, this, std::placeholders::_1)
            }
        }
    };
};

#endif // __CLI_H__
