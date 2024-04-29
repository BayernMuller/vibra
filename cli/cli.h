#ifndef __CLI_H__
#define __CLI_H__

#include <string>
#include <map>
#include <functional>
#include "../fingerprinting/algorithm/signature.h"
#include "../fingerprinting/audio/wav.h"

// Forward declaration
class Wav;

class CLI 
{
public:
    int Run(int argc, char** argv);

private:
    Signature getSignatureFromPcm(const Raw16bitPCM& pcm);
    Raw16bitPCM getPcmFromStdin(int chunk_seconds, int sample_rate, int channels, int bits_per_sample);
};

#endif // __CLI_H__
