#include <emscripten/emscripten.h>
#include "../fingerprinting/algorithm/signature_generator.h"

#ifdef __cplusplus
extern "C" {
#endif

char* gSignature = nullptr;

char* EMSCRIPTEN_KEEPALIVE GetSignature(char* pcm, int sample_rate, int channels, int pcm_data_size, int bitpersample)
{
    gSignature = new char[10]{0};
    memcpy(gSignature, "123456789", 9);
    return gSignature;
}

void EMSCRIPTEN_KEEPALIVE FreeSignature()
{
    if (gSignature != nullptr)
    {
        delete[] gSignature;
        gSignature = nullptr;
    }
}

int main()
{
    return 0; // does nothing
}

#ifdef __cplusplus
}
#endif
