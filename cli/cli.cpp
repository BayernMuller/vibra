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

    if (wav_file)
    {
        if (fingerprint)
        {
            std::cout << fingerprintFromWavFile(args::get(wav_file)) << std::endl;
        }
        else if (recognize)
        {
            std::cout << recognizeSongFromWavFile(args::get(wav_file)) << std::endl;
        }
    }
    else
    {
        if (fingerprint)
        {
            std::cout << fingerprintFromRawPCM(args::get(chunk_seconds)) << std::endl;
        }
        else if (recognize)
        {
            std::cout << recognizeSongFromRawPCM(args::get(chunk_seconds)) << std::endl;
        }
    }
}

CLI::~CLI()
{
}

void CLI::Run()
{
    
}

std::string CLI::fingerprintFromWavFile(std::string filepath)
{
    auto signature = getSignatureFromWavFile(filepath);
    return signature.GetBase64Uri();
}

std::string CLI::fingerprintFromRawPCM(int chunk_seconds)
{
    auto signature = getSignatureFromRawPCM(chunk_seconds);
    return signature.GetBase64Uri();
}

std::string CLI::recognizeSongFromWavFile(std::string filepath)
{
    auto signature = getSignatureFromWavFile(filepath);
    return Shazam::RequestMetadata(signature);
}

std::string CLI::recognizeSongFromRawPCM(int chunk_seconds)
{
    auto signature = getSignatureFromRawPCM(chunk_seconds);
    return Shazam::RequestMetadata(signature);
}

Signature CLI::getSignatureFromWavFile(std::string filepath)
{
    Wav wav(filepath);
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

Signature CLI::getSignatureFromRawPCM(int chunk_seconds)
{
    int samples = LOW_QUALITY_SAMPLE_RATE * chunk_seconds;
    if (samples <= 0)
        throw std::runtime_error("Invalid chunk_seconds");

    Raw16bitPCM raw_pcm(samples);
    std::cin.read((char*)raw_pcm.data(), samples * sizeof(Sample));
    SignatureGenerator generator;
    generator.FeedInput(raw_pcm);
    generator.SetMaxTimeSeconds(chunk_seconds);

    return generator.GetNextSignature();
}
