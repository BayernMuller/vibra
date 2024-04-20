#include <iostream>
#include "cli.h"
#include "args/args.hxx"
#include "../fingerprinting/audio/wav.h"
#include "../fingerprinting/algorithm/signature_generator.h"
#include "../communication/shazam.h"

CLI::CLI(int argc, char** argv)
{
    args::ArgumentParser parser("");
    parser.SetArgumentSeparations(false, false, true, true);

    parser.helpParams.width = 200;
    parser.helpParams.progindent = 0;
    parser.helpParams.progtailindent = 0;
    parser.helpParams.flagindent = 2;
    parser.helpParams.descriptionindent = 2;
    parser.helpParams.eachgroupindent = 4;
    parser.helpParams.showValueName = false;
    parser.helpParams.optionsString = "Options:";
    parser.helpParams.proglineOptions = "{COMMAND} [OPTIONS]";
    
    args::Group actions(parser, "Commands:", args::Group::Validators::Xor);
    args::Flag fingerprint(actions, "fingerprint", "Generate a fingerprint", {'F', "fingerprint"});
    args::Flag recognize(actions, "recognize", "Recognize a song", {'R', "recognize"});
    args::HelpFlag help(actions, "help", "Display this help menu", {'h', "help"});
   
    args::Group sources(parser, "Sources:", args::Group::Validators::Xor);

    args::Group file_sources(sources, "File sources:", args::Group::Validators::Xor);
    args::ValueFlag<std::string> wav_file(file_sources, "file", "WAV file", {'w', "wav"});
    
    args::Group raw_sources(sources, "Raw PCM sources:", args::Group::Validators::All);
    args::ValueFlag<int> chunk_seconds(raw_sources, "seconds", "Chunk seconds", {'s', "seconds"});
    args::ValueFlag<int> sample_rate(raw_sources, "rate", "Sample rate", {'r', "rate"});
    args::ValueFlag<int> channels(raw_sources, "channels", "Channels", {'c', "channels"});
    args::ValueFlag<int> bits_per_sample(raw_sources, "bits", "Bits per sample", {'b', "bits"});

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        exit(0);
    }
    catch (args::ParseError e)
    {
        std::cerr << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << std::endl;
        std::cerr << parser;
        exit(1);
    }
    catch (args::ValidationError e)
    {
        std::cerr << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << std::endl;
        std::cerr << parser;
        exit(1);
    }

    Wav *wav = nullptr;
    if (wav_file)
    {
        wav = new Wav(args::get(wav_file));
    }
    else if (chunk_seconds && sample_rate && channels && bits_per_sample)
    {
        wav = getWavFromStdin(args::get(chunk_seconds), args::get(sample_rate), args::get(channels), args::get(bits_per_sample));
    }
    else
    {
        std::cerr << "Invalid arguments" << std::endl;
        exit(1);
    }
    
    Signature signature = getSignatureFromWav(*wav);
    if (fingerprint)
    {
        std::cout << signature.GetBase64Uri() << std::endl;
    }
    else if (recognize)
    {
        std::cout << Shazam::RequestMetadata(signature) << std::endl;
    }
    delete wav;
}

CLI::~CLI()
{
}

void CLI::Run()
{
    
}

Signature CLI::getSignatureFromWav(const Wav& wav)
{
    Raw16bitPCM pcm;
    wav.GetLowQualityPCM(&pcm);
    SignatureGenerator generator;
    generator.FeedInput(pcm);
    generator.SetMaxTimeSeconds(12);
    auto duaration = pcm.size() / LOW_QUALITY_SAMPLE_RATE;
    if (duaration > 12 * 3)
        generator.AddSampleProcessed(LOW_QUALITY_SAMPLE_RATE * ((int)duaration / 2) - 6);

    return generator.GetNextSignature();
}

Wav* CLI::getWavFromStdin(int chunk_seconds, int sample_rate, int channels, int bits_per_sample)
{
    std::size_t bytes = chunk_seconds * sample_rate * channels * (bits_per_sample / 8);
    std::vector<char> buffer(bytes);
    std::cin.read(buffer.data(), bytes);
    return new Wav(buffer.data(), bytes, sample_rate, bits_per_sample, channels);
}
