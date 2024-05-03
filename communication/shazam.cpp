#include "shazam.h"
#include "user_agents.h"
#include "timezones.h"
#include "../fingerprinting/utils/uuid4.h"
#include "../fingerprinting/algorithm/signature.h"
#include <random>
#include <sstream>
#include <curl/curl.h>
#include <algorithm>

// static variables initialization
constexpr char Shazam::HOST[];

// static variables
static std::string read_buffer;

// static functions
static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    (void)userp; // suppress warning (unused parameter)
    std::size_t realsize = size * nmemb;
    read_buffer.append((char*)contents, realsize);
    return realsize;
}

std::string Shazam::RequestMetadata(const Signature& signature)
{
    auto content = getRequestContent(signature);
    auto user_agent = getUserAgent();

    std::string url = HOST + uuid4::generate() + "/" + uuid4::generate();
    url += "?sync=true&webv3=true&sampling=true&connected=&shazamapiversion=v3&sharehub=true&video=v3";

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
        curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
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

        int http_code = 0;
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

std::string Shazam::getRequestContent(const Signature& signature)
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis_float(0.0, 1.0);
    auto uri = signature.GetBase64Uri();
    auto timezone = getTimezone(); 
    double fuzz = dis_float(gen) * 15.3 - 7.65;
    
    std::stringstream json_buf;
    json_buf << "{";
    json_buf << "\"geolocation\":{";
    json_buf << "\"altitude\":" << dis_float(gen) * 400 + 100 + fuzz << ",";
    json_buf << "\"latitude\":" << dis_float(gen) * 180 - 90 + fuzz << ",";
    json_buf << "\"longitude\":" << dis_float(gen) * 360 - 180 + fuzz;
    json_buf << "},";
    json_buf << "\"signature\":{";
    json_buf << "\"samplems\":" << int(signature.NumberOfSamples() / signature.SampleRate() * 1000) << ",";
    json_buf << "\"timestamp\":" << time(nullptr) * 1000ULL << ",";
    json_buf << "\"uri\":\"" << uri << "\"";
    json_buf << "},";
    json_buf << "\"timestamp\":" << time(nullptr) * 1000ULL << ",";
    json_buf << "\"timezone\":" << "\"" << timezone << "\"";
    json_buf << "}";
    std::string content = json_buf.str();
    return content;
}

std::string Shazam::getUserAgent()
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis_useragent(0, USER_AGENTS_SIZE - 1);
    return USER_AGENTS[dis_useragent(gen)];
}

std::string Shazam::getTimezone()
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis_timezone(0, EUROPE_TIMEZONES_SIZE - 1);
    return EUROPE_TIMEZONES[dis_timezone(gen)];
}