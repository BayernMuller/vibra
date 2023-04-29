#include <iostream>
#include "../fingerprinting/audio/wav.h"
#include "../fingerprinting/algorithm/signature_generator.h"

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <audio_file>" << std::endl;
        return 1;
    }

    Wav wav(argv[1]);
    cout << "Sample rate: " << wav.GetSampleRate() << endl;
    cout << "Num channels: " << wav.GetChannel() << endl;
    cout << "Num bit per sample: " << wav.GetBitPerSample() << endl;
    
    Raw16bitPCM pcm;
    wav.GetLowQualityPCM(pcm);
    cout << "Num samples: " << pcm.size() << endl;

    SignatureGenerator generator;
    generator.FeedInput(pcm);
    Signature signature = generator.GetNextSignature();
}