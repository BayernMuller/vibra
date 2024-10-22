#ifndef LIB_COMMUNICATION_SHAZAM_H_
#define LIB_COMMUNICATION_SHAZAM_H_

#include <string>

class Shazam
{
    static constexpr char HOST[] = "https://amp.shazam.com/discovery/v5/fr/FR/android/-/tag/";

  public:
    static std::string GetShazamHost();
    static std::string GetUserAgent();
    static std::string GetRequestContent(const std::string &uri, unsigned int sample_ms);

  private:
    static std::string getTimezone();
};

#endif // LIB_COMMUNICATION_SHAZAM_H_
