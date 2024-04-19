#include "shazam.h"
#include "user_agents.h"
#include "timezones.h"
#include "../fingerprinting/utils/uuid4.h"
#include "../fingerprinting/algorithm/signature.h"
#include <random>
#include <sstream>
#include <curl/curl.h>
#include <algorithm>

static const char* SHAZAM_ENDPOINT = "https://amp.shazam.com/discovery/v5/fr/FR/android/-/tag/";    

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string Shazam::RequestMetadata(const Signature& signature)
{
    auto uuid1 = uuid4::generate();
    auto uuid2 = uuid4::generate();
    std::transform(uuid1.begin(), uuid1.end(), uuid1.begin(), ::toupper);
    
    std::string url = SHAZAM_ENDPOINT + uuid1 + "/" + uuid2;
    url += "?sync=true&webv3=true&sampling=true&connected=&shazamapiversion=v3&sharehub=true&video=v3";
    
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis_useragent(0, USER_AGENTS_SIZE - 1);
    std::uniform_int_distribution<> dis_float(0.0, 1.0);
    std::uniform_int_distribution<> dis_timezone(0, EUROPE_TIMEZONES_SIZE - 1);
    std::string user_agent = USER_AGENTS[dis_useragent(gen)];
    std::string timezone = EUROPE_TIMEZONES[dis_timezone(gen)];
    std::string uri = signature.GetBase64Uri(); 

    double fuzz = dis_float(gen) * 15.3 - 7.65;
    user_agent = "User-Agent: " + user_agent;

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
    std::string json_str = json_buf.str();
    
    CURL* curl = curl_easy_init();
    std::string readBuffer;

    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, user_agent.c_str());
        headers = curl_slist_append(headers, "Accept-Encoding: gzip, deflate, br");
        headers = curl_slist_append(headers, "Accept: */*");
        headers = curl_slist_append(headers, "Connection: keep-alive");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Content-Language: en_US");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip, deflate, br");
        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200) {
            fprintf(stderr, "curl_easy_perform() failed: %ld\n", http_code); 
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return readBuffer; // Return the response
}