#include "communication/shazam.h"
#include <random>
#include <sstream>
#include <algorithm>
#include "communication/user_agents.h"
#include "communication/timezones.h"
#include "utils/uuid4.h"
#include "algorithm/signature.h"

// static variables initialization
constexpr char Shazam::HOST[];

std::string Shazam::GetShazamHost()
{
    std::string host = HOST + uuid4::generate() + "/" + uuid4::generate();
    host += "?sync=true&"
            "webv3=true&"
            "sampling=true&"
            "connected=&"
            "shazamapiversion=v3&"
            "sharehub=true&"
            "video=v3";
    return host;
}

std::string Shazam::GetRequestContent(const std::string& uri, unsigned int sample_ms)
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis_float(0.0, 1.0);

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
    json_buf << "\"samplems\":" << sample_ms << ",";
    json_buf << "\"timestamp\":" << time(nullptr) * 1000ULL << ",";
    json_buf << "\"uri\":\"" << uri << "\"";
    json_buf << "},";
    json_buf << "\"timestamp\":" << time(nullptr) * 1000ULL << ",";
    json_buf << "\"timezone\":" << "\"" << timezone << "\"";
    json_buf << "}";
    std::string content = json_buf.str();
    return content;
}

std::string Shazam::GetUserAgent()
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
