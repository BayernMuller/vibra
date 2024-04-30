#include <iostream>
#include "cli.h"
#include "args/args.hxx"
#include "../fingerprinting/algorithm/signature_generator.h"
#include "../fingerprinting/utils/ffmpeg.h"
#include "../communication/shazam.h"

int CLI::Run(int argc, char** argv)
{
    using namespace ffmpeg;

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
    args::ValueFlag<std::string> music_file(file_sources, "file", "FFmpeg required for non-wav files", {'f', "file"});
    
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
        return 0;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << std::endl;
        std::cerr << parser;
        return 1;
    }

    Raw16bitPCM pcm;
    if (music_file)
    {
        std::string file = args::get(music_file);
        if (file.size() >= 4 && file.substr(file.size() - 4) == ".wav")
        {
            Wav wav(file);
            wav.GetLowQualityPCM(&pcm);
        }
        else
        {
            FFmpegWrapper::convertToWav(file, &pcm);             
        }
    }
    else if (chunk_seconds && sample_rate && channels && bits_per_sample)
    {
        pcm = getPcmFromStdin(args::get(chunk_seconds), args::get(sample_rate), args::get(channels), args::get(bits_per_sample));
    }
    else
    {
        std::cerr << "Invalid arguments" << std::endl;
        return 1;   
    }
    
    Signature signature = getSignatureFromPcm(pcm);
    if (fingerprint)
    {
        std::cout << signature.GetBase64Uri() << std::endl;
    }
    else if (recognize)
    {
        std::cout << Shazam::RequestMetadata(signature) << std::endl;
    }
    return 0;
}

Signature CLI::getSignatureFromPcm(const Raw16bitPCM& pcm)
{
    SignatureGenerator generator;
    generator.FeedInput(pcm);
    generator.SetMaxTimeSeconds(12);
    auto duaration = pcm.size() / LOW_QUALITY_SAMPLE_RATE;
    if (duaration > 12 * 3)
        generator.AddSampleProcessed(LOW_QUALITY_SAMPLE_RATE * ((int)duaration / 2) - 6);

    return generator.GetNextSignature();
}

Raw16bitPCM CLI::getPcmFromStdin(int chunk_seconds, int sample_rate, int channels, int bits_per_sample)
{
    std::size_t bytes = chunk_seconds * sample_rate * channels * (bits_per_sample / 8);
    std::vector<char> buffer(bytes);
    std::cin.read(buffer.data(), bytes);
    Wav wav(buffer.data(), bytes, sample_rate, bits_per_sample, channels);
    Raw16bitPCM pcm;
    wav.GetLowQualityPCM(&pcm);
    return pcm;
}
