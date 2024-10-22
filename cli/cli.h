#ifndef CLI_CLI_H_
#define CLI_CLI_H_

#include <string>
#include "../include/vibra.h"

class CLI
{
public:
    int Run(int argc, char** argv);

private:
    Fingerprint* getFingerprintFromMusicFile(const std::string& music_file);
    Fingerprint* getFingerprintFromStdin(int chunk_seconds, int sample_rate,
                            int channels, int bits_per_sample, bool is_signed);
    std::string getMetadataFromShazam(const Fingerprint* fingerprint);
};

#endif // CLI_CLI_H_
