#include "shazam.h"
#include "user_agents.h"
#include "timezones.h"
#include "../utils/uuid4.h"
#include "../algorithm/signature.h"
#include <random>
#include <sstream>
#include <curl/curl.h>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string Shazam::RequestMetadata(const Signature& signature)
{
    static const char* SHAZAM_ENDPOINT = "https://amp.shazam.com/discovery/v5/fr/FR/android/-/tag/";    
    std::string url = SHAZAM_ENDPOINT + uuid4::generate() + "/" + uuid4::generate();
    url += "?sync=true&webv3=true&sampling=true&connected=&shazamapiversion=v3&sharehub=true&video=v3";

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis_useragent(0, USER_AGENTS_SIZE - 1);
    std::uniform_int_distribution<> dis_float(0.0, 1.0);
    std::uniform_int_distribution<> dis_timezone(0, EUROPE_TIMEZONES_SIZE - 1);
    std::string uger_agent = USER_AGENTS[dis_useragent(gen)];
    std::string timezone = EUROPE_TIMEZONES[dis_timezone(gen)];
    std::string uri;
    signature.GetBase64Uri(uri);

    double fuzz = dis_float(gen) * 15.3 - 7.65;

    std::stringstream json;
    json << "{";
        json << "\"geolocation\":{";
            json << "\"altitude\":" << dis_float(gen) * 400 + 100 + fuzz << ",";
            json << "\"latitude\":" << dis_float(gen) * 180 - 90 + fuzz << ",";
            json << "\"longitude\":" << dis_float(gen) * 360 - 180 + fuzz;
        json << "},";
        json << "\"signature\":{";
            json << "\"samples\":" << int(signature.NumberOfSamples() / signature.SampleRate() * 1000) << ",";
            json << "\"time\":" << time(nullptr) * 1000ULL << ",";
            json << "\"uri\":\"" << uri << "\"";
        json << "},";
        json << "\"timestamp\":" << time(nullptr) * 1000ULL << ",";
        json << "\"timezone\":" << "\"" << timezone << "\"";
    json << "}";

    std::cout << json.str() << std::endl;

    CURL* curl = curl_easy_init();
    std::string readBuffer; // To store the response

    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, std::string("Content-Language: en-US").c_str());
        headers = curl_slist_append(headers, std::string("Content-Type: application/json").c_str());
        headers = curl_slist_append(headers, std::string("User-Agent: " + uger_agent).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.str().c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer); // Set the write callback
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // get status code and response
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code == 200) {
            // The response is now in readBuffer
        } else {
            fprintf(stderr, "HTTP status code: %ld\n", http_code);
            fprintf(stderr, "HTTP request failed: %s\n", curl_easy_strerror(res));
        }

        curl_slist_free_all(headers); // Free the header list
        curl_easy_cleanup(curl);
    } 

    return readBuffer; // Return the response
}