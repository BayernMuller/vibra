#include <iostream>
#include <string>
#include "fingerprinting/audio/wav.h"
#include "fingerprinting/algorithm/signature_generator.h"
#include "fingerprinting/communication/shazam.h"

using namespace std;

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
    string base64Uri = signature.GetBase64Uri();
    
    auto res = Shazam::RequestMetadata(signature);
    cout << res << endl;
}