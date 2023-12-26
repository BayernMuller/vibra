#include <iostream>
#include <string>
#include "../fingerprinting/audio/wav.h"
#include "../fingerprinting/algorithm/signature_generator.h"
#include "../fingerprinting/utils/array.h"

using namespace std;
using namespace array;

std::string ToString(FrequancyBand band)
{
    switch (band)
    {
    case FrequancyBand::_0_150:
        return "0_150";
    case FrequancyBand::_250_520:
        return "250_520";
    case FrequancyBand::_520_1450:
        return "520_1450";
    case FrequancyBand::_1450_3500:
        return "1450_3500";
    case FrequancyBand::_3500_5500:
        return "3500_5500";
    default:
        return "Unknown";
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <audio_file>" << std::endl;
        return 1;
    }

    Wav wav(argv[1]);
    
    Raw16bitPCM pcm;
    wav.GetLowQualityPCM(pcm);

    SignatureGenerator generator;
    generator.FeedInput(pcm);

    generator.SetMaxTimeSeconds(12);
    auto duaration = pcm.size() / LOW_QUALITY_SAMPLE_RATE;
    if (duaration > 12 * 3)
        generator.AddSampleProcessed(LOW_QUALITY_SAMPLE_RATE * ((int)duaration / 2) - 6);

    Signature signature = generator.GetNextSignature();
    string base64Uri;
    signature.GetBase64Uri(base64Uri);
    cout << base64Uri;
}