#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include "cli.h"
#include "args/args.hxx"

static std::string read_buffer;

std::size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    (void)userp; // suppress warning (unused parameter)
    std::size_t realsize = size * nmemb;
    read_buffer.append((char*)contents, realsize);
    return realsize;
}

int CLI::Run(int argc, char** argv)
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
    args::Flag fingerprint_only(actions, "fingerprint", "Generate a fingerprint", {'F', "fingerprint"});
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
    catch (const args::Help&)
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

    Fingerprint* fingerprint = nullptr;
    if (music_file)
    {
        std::string file = args::get(music_file);
        if (std::ifstream(file).good() == false)
        {
            std::cerr << "File not found: " << file << std::endl;
            return 1;
        }

        fingerprint = vibra_get_fingerprint_from_music_file(file.c_str());
    }
    else if (chunk_seconds && sample_rate && channels && bits_per_sample)
    {
        fingerprint = getFingerprintFromStdin(
            args::get(chunk_seconds),
            args::get(sample_rate),
            args::get(channels),
            args::get(bits_per_sample));
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
        std::cout << getMetadataFromShazam(fingerprint) << std::endl;
    }
    return 0;
}

Fingerprint* CLI::getFingerprintFromStdin(int chunk_seconds, int sample_rate, int channels, int bits_per_sample)
{
    std::size_t bytes = chunk_seconds * sample_rate * channels * (bits_per_sample / 8);
    std::vector<char> buffer(bytes);
    std::cin.read(buffer.data(), bytes);
    return vibra_get_fingerprint_from_pcm(buffer.data(), bytes, sample_rate, bits_per_sample, channels);
}

std::string CLI::getMetadataFromShazam(const Fingerprint* fingerprint)
{
    auto content = vibra_get_shazam_request_json(fingerprint);
    auto user_agent = vibra_get_shazam_random_user_agent();
    std::string url = vibra_get_shazam_host();

    CURL* curl = curl_easy_init();
    read_buffer.clear();

    if (curl) 
    {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Accept-Encoding: gzip, deflate, br");
        headers = curl_slist_append(headers, "Accept: */*");
        headers = curl_slist_append(headers, "Connection: keep-alive");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Content-Language: en_US");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
        
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip, deflate, br");
        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
        
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) 
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200) 
        {
            std::cerr << "HTTP code: " << http_code << std::endl;
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return read_buffer;
}