#ifndef __CLI_H__
#define __CLI_H__

#include <string>
#include <map>
#include <functional>
#include "../fingerprinting/algorithm/signature.h"

class CLI 
{
public:
    CLI(int argc, char** argv);
    ~CLI();
    void Run();

private:
    std::string fingerprintFromWavFile(std::string filepath);
    std::string recognizeSongFromWavFile(std::string filepath);

    std::string fingerprintFromRawPCM(int chunk_seconds);
    std::string recognizeSongFromRawPCM(int chunk_seconds);

private:
    Signature getSignatureFromWavFile(std::string filepath);
    Signature getSignatureFromRawPCM(int chunk_seconds);
};

#endif // __CLI_H__
