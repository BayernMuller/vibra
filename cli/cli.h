#ifndef __CLI_H__
#define __CLI_H__

#include <string>
#include <map>
#include <functional>
#include "../fingerprinting/algorithm/signature.h"

// Forward declaration
class Wav;

class CLI 
{
public:
    CLI(int argc, char** argv);
    ~CLI();

private:
    Signature getSignatureFromWav(const Wav& wav);
    Wav* getWavFromStdin(int chunk_seconds, int sample_rate, int channels, int bits_per_sample);
};

#endif // __CLI_H__
