#ifndef __CLI_H__
#define __CLI_H__

#include <string>
#include <map>
#include <functional>
#include "../fingerprinting/algorithm/signature.h"

class CLIMain 
{
public:
    CLIMain(int argc, char** argv);
    ~CLIMain();
    void Run();

private:
    void help();
    std::string fingerprintingByFile(std::string filepath);
    std::string fingerprintingByMicrophone(std::string microphone) { return ""; } // TODO: implement
    std::string recognizeSongByFile(std::string filepath);
    std::string recognizeSongByFingerprint(std::string fingerprint) { return ""; } // TODO: implement
    std::string recognizeSongByMicrophone(std::string microphone) { return ""; } // TODO: implement

    Signature GetSignatureFromFile(std::string filepath);
private:
    std::string m_action;
    std::string m_param;
    std::map<std::string, std::function<std::string(std::string)>> m_commands{
        {"fingerprinting-by-file", std::bind(&CLIMain::fingerprintingByFile, this, std::placeholders::_1)},
        {"recognize-song-by-file", std::bind(&CLIMain::recognizeSongByFile, this, std::placeholders::_1)},
        //{"fingerprinting-by-mic", std::bind(&CLIMain::fingerprintingByMicrophone, this, std::placeholders::_1)},
        //{"recognize-song-by-fingerprint", std::bind(&CLIMain::recognizeSongByFingerprint, this, std::placeholders::_1)},
        //{"recognize-song-by-microphone", std::bind(&CLIMain::recognizeSongByMicrophone, this, std::placeholders::_1)}
    };
};

#endif // __CLI_H__
