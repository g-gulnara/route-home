#include <cpr/cpr.h>
#include <cinttypes>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

using json = nlohmann::json;
namespace fs = std::filesystem;
#pragma once

const std::string kKey = "key.txt";
const uint32_t kMaxNumberOfCacheFiles = 50;
const std::string kCacheDir = "cache/";
const auto kExpirationTime = std::chrono::hours(24);

struct Place {
    std::string code;
    std::string popular_title;
    std::string short_title;
    std::string station_type;
    std::string station_type_name;
    std::string title;
    std::string transport_type;
    std::string type;
};

struct Route {
    Place from;
    Place to;
    std::string arrival;
    std::string arrival_platform;
    std::string arrival_terminal;
    std::string departure;
    std::string departure_platform;
    std::string departure_terminal;
    bool has_transfers;
    std::string start_date;
    Place transfer;
};

struct Search {
    std::string date;
    Place from;
    Place to;  
};

struct Response {
    Search search;
    std::vector <Route*> routes;

    ~Response();
};

bool CheckDataFormat(char* str);

std::string ParseArguments(int argc, char** argv);
json GetPaths(const std::string& town_from, const std::string& town_to, 
                        const std::string& date);

json LoadCache(const std::string& from, const std::string& to, 
    const std::string& date);

bool Process(const std::string& date, const std::string& from, 
    const std::string& to);
Response FindRoutes(const std::string& from, const std::string& to, 
    const std::string& date);

json LoadCache(const std::string& from, const std::string& to, 
    const std::string& date);
std::string GetCacheFilename(const std::string& from, const std::string& to,
    const std::string& date);

std::string SafeGet(json& j, std::string key);

void RemoveFiles();

bool Comp(const fs::directory_entry& a, const fs::directory_entry& b);