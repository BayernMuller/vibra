#include <iostream>
#include "cli.h"
#include "../fingerprinting/audio/wav.h"
#include "../fingerprinting/algorithm/signature_generator.h"
#include "../fingerprinting/communication/shazam.h"

CLIMain::CLIMain(int argc, char** argv)
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

CLIMain::~CLIMain()
{
}

void CLIMain::Run()
{
    std::cout << m_commands[m_action](m_param) << std::endl;
}

std::string CLIMain::fingerprintingByFile(std::string filepath)
{
    auto signature = GetSignatureFromFile(filepath);
    return signature.GetBase64Uri();
}

std::string CLIMain::recognizeSongByFile(std::string filepath)
{
    auto signature = GetSignatureFromFile(filepath);
    return Shazam::RequestMetadata(signature);
}

Signature CLIMain::GetSignatureFromFile(std::string filepath)
{
    Wav wav(filepath);
    Raw16bitPCM pcm;
    wav.GetLowQualityPCM(pcm);
    SignatureGenerator generator;
    generator.FeedInput(pcm);
    generator.SetMaxTimeSeconds(12);
    auto duaration = pcm.size() / LOW_QUALITY_SAMPLE_RATE;
    if (duaration > 12 * 3)
        generator.AddSampleProcessed(LOW_QUALITY_SAMPLE_RATE * ((int)duaration / 2) - 6);

    return generator.GetNextSignature();
}

void CLIMain::help()
{
    std::cout << std::endl;
    std::cout << "Usage: vibra <action> <param>" << std::endl;
    std::cout << std::endl;
    std::cout << "* Actions:" << std::endl;
    for (auto& command : m_commands)
    {
        std::cout << "\t" << command.first << " <param>" << std::endl;
    }
    std::cout << std::endl;
}