#include "../cli/cli.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <args.hxx>
#include "communication/shazam.h"

int CLI::Run(int argc, char **argv)
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
    args::Flag fingerprint_only(actions, "fingerprint", "Generate a fingerprint",
                                {'F', "fingerprint"});
    args::Flag recognize(actions, "recognize", "Recognize a song", {'R', "recognize"});
    args::HelpFlag help(actions, "help", "Display this help menu", {'h', "help"});

    args::Group sources(parser, "Sources:", args::Group::Validators::Xor);

    args::Group file_sources(sources, "File sources:", args::Group::Validators::Xor);
    args::ValueFlag<std::string> music_file(file_sources, "file",
                                            "FFmpeg required for non-wav files", {'f', "file"});

    args::Group raw_sources(sources, "Raw PCM sources:", args::Group::Validators::All);
    args::ValueFlag<int> chunk_seconds(raw_sources, "seconds", "Chunk seconds", {'s', "seconds"});
    args::ValueFlag<int> sample_rate(raw_sources, "rate", "Sample rate", {'r', "rate"});
    args::ValueFlag<int> channels(raw_sources, "channels", "Channels", {'c', "channels"});
    args::ValueFlag<int> bits_per_sample(raw_sources, "bits", "Bits per sample", {'b', "bits"});

    args::Group source_type(raw_sources, "Source type:", args::Group::Validators::AtMostOne);
    args::Flag signed_pcm(source_type, "signed", "Signed PCM (default)", {'S', "signed"});
    args::Flag float_pcm(source_type, "float", "Float PCM", {'D', "float"});

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (const args::Help &)
    {
        std::cout << parser;
        return 0;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << std::endl;
        std::cerr << parser;
        return 1;
    }

    Fingerprint *fingerprint = nullptr;
    if (music_file)
    {
        std::string file = args::get(music_file);
        fingerprint = getFingerprintFromMusicFile(file);
    }
    else if (chunk_seconds && sample_rate && channels && bits_per_sample)
    {
        bool is_signed = signed_pcm || !float_pcm;
        fingerprint =
            getFingerprintFromStdin(args::get(chunk_seconds), args::get(sample_rate),
                                    args::get(channels), args::get(bits_per_sample), is_signed);
    }
    else
    {
        std::cerr << "Invalid arguments" << std::endl;
        return 1;
    }

    if (fingerprint_only)
    {
        std::cout << fingerprint->uri << std::endl;
    }
    else if (recognize)
    {
        std::cout << Shazam::Recognize(fingerprint) << std::endl;
    }
    return 0;
}

Fingerprint *CLI::getFingerprintFromMusicFile(const std::string &music_file)
{
    if (std::ifstream(music_file).good() == false)
    {
        std::cerr << "File not found: " << music_file << std::endl;
        throw std::ifstream::failure("File not found");
    }
    return vibra_get_fingerprint_from_music_file(music_file.c_str());
}

Fingerprint *CLI::getFingerprintFromStdin(int chunk_seconds, int sample_rate, int channels,
                                          int bits_per_sample, bool is_signed)
{
    std::size_t bytes = chunk_seconds * sample_rate * channels * (bits_per_sample / 8);
    std::vector<char> buffer(bytes);
    std::cin.read(buffer.data(), bytes);
    if (is_signed)
    {
        return vibra_get_fingerprint_from_signed_pcm(buffer.data(), bytes, sample_rate,
                                                     bits_per_sample, channels);
    }
    return vibra_get_fingerprint_from_float_pcm(buffer.data(), bytes, sample_rate, bits_per_sample,
                                                channels);
}
