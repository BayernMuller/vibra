#ifndef __SHAZAM_H__
#define __SHAZAM_H__

#include <string>

// forward declaration
class Signature;

class Shazam 
{
    static constexpr char HOST[] = "https://amp.shazam.com/discovery/v5/fr/FR/android/-/tag/";

public:
    static std::string GetShazamHost();
    static std::string GetUserAgent();
    static std::string GetRequestContent(const std::string& uri, unsigned int sample_ms);

private:
    static std::string getTimezone();
};


#endif // __SHAZAM_H__