#include <iostream>
#include "cli.h"
#include "../fingerprinting/audio/wav.h"
#include "../fingerprinting/algorithm/signature_generator.h"
#include "../communication/shazam.h"

CLI::CLI(int argc, char** argv)
{
    if (argc < 3)
    {
        help();
        std::exit(1);
    }

    m_action = argv[1];
    m_param = argv[2];
    
    if (m_commands.find(m_action) == m_commands.end())
    {
        help();
        std::exit(1);
    }
}

CLI::~CLI()
{
}

void CLI::Run()
{
    std::cout << m_commands[m_action].func(m_param) << std::endl;
}

std::string CLI::fingerprintFromWavFile(std::string filepath)
{
    auto signature = getSignatureFromWavFile(filepath);
    return signature.GetBase64Uri();
}

std::string CLI::fingerprintFromRawPCM(std::string chunk_seconds)
{
    std::size_t seconds = std::stoi(chunk_seconds);
    auto signature = getSignatureFromRawPCM(seconds);
    return signature.GetBase64Uri();
}

std::string CLI::recognizeSongFromWavFile(std::string filepath)
{
    auto signature = getSignatureFromWavFile(filepath);
    return Shazam::RequestMetadata(signature);
}

std::string CLI::recognizeSongFromRawPCM(std::string chunk_seconds)
{
    std::size_t seconds = std::stoi(chunk_seconds);
    auto signature = getSignatureFromRawPCM(seconds);
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

void CLI::help()
{
    std::cout << std::endl;
    std::cout << "Usage: vibra <action> <param>" << std::endl;
    std::cout << std::endl;
    std::cout << "* Actions:" << std::endl;
    for (auto& command : m_commands)
    {
        std::cout << "\t" << command.first << " <" << command.second.param << ">" << std::endl;
        std::cout << "\t\t" << command.second.description << std::endl;
        std::cout << std::endl;
    }
    std::cout << std::endl;
}