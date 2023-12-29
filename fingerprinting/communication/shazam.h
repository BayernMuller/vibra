#ifndef __SHAZAM_H__
#define __SHAZAM_H__

#include <string>

// forward declaration
class Signature;

class Shazam 
{
public:
    static std::string RequestMetadata(const Signature& signature);

private:
    static std::string getUserAgent();
};


#endif // __SHAZAM_H__