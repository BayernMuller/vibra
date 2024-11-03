#ifndef CLI_COMMUNICATION_SHAZAM_H_
#define CLI_COMMUNICATION_SHAZAM_H_

#include <string>

// forward declaration
struct Fingerprint;
//

class Shazam
{
    static constexpr char HOST[] = "https://amp.shazam.com/discovery/v5/fr/FR/android/-/tag/";

public:
    static std::string Recognize(const Fingerprint *fingerprint);

private:
    static std::string getShazamHost();
    static std::string getUserAgent();
    static std::string getRequestContent(const std::string &uri, unsigned int sample_ms);
    static std::string getTimezone();
};

#endif // CLI_COMMUNICATION_SHAZAM_H_
