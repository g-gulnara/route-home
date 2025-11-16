#include "way.h"

bool CheckDataFormat(char* str) {
    int first_dash_position = 4;
    int second_dash_position = 7;
    
    if (str[first_dash_position] == '-' && str[second_dash_position] == '-') {
        return true;
    }
     
    return false;
}

std::string ParseArguments(int argc, char** argv) {
    if (argc != 2 || !CheckDataFormat) {
        return "";
    }

    return argv[1];
}

std::string SafeGet(json& j, std::string key) {
    if (!j.contains(key)) {
        return "";
    }
    return j[key];
}

void PrintResponse(Response* response) {
    std::cout << "---------------------------------\n";
    std::cout << response->search.date << ' ' << response->search.from.title 
                                << ' ' << response->search.to.title << '\n';
    std::cout << "---------------------------------\n";
    for (const auto& route : response->routes) {
        if (route->has_transfers) {
            std::cout << route->from.title << ' ' << route->arrival << ' '
                      << route->arrival_platform << ' ' << route->transfer.title
                      << ' ' << route->departure << ' ' << route->departure_platform
                      << ' ' << route->to.title << '\n';
        } else {
            std::cout << route->from.title << ' ' << route->arrival << ' '
                      << route->arrival_platform << ' ' << route->departure << ' '
                      << route->departure_platform << ' ' << route->to.title << '\n';
        }
   
    }
}
 
bool Process(const std::string& date, const std::string& from, 
                                        const std::string& to) {
    RemoveFiles();

    Response petersburg_chelny = FindRoutes(from, to, date);
    Response chelny_petersburg = FindRoutes(to, from, date);

    PrintResponse(&petersburg_chelny);
    PrintResponse(&chelny_petersburg);

    return true;
}

std::string GetCacheFilename(const std::string& from, const std::string& to,
    const std::string& date) {
    
    return kCacheDir + "cache_" + from + "_" + to + "_" + date + ".json";
}

bool Comp(const fs::directory_entry& first_file, const 
                    fs::directory_entry& second_file) {
    return fs::last_write_time(first_file) < fs::last_write_time(second_file);
}

void RemoveFiles() {
    std::vector<fs::directory_entry> files;

    if (!fs::exists(kCacheDir)) {
        fs::create_directory(kCacheDir);
    }

    auto now = fs::file_time_type::clock::now();

    for (const auto& entry : fs::directory_iterator(kCacheDir)) {
        if (entry.is_regular_file()) {
            auto last_write = fs::last_write_time(entry);
            if (now - last_write > kExpirationTime) {
                fs::remove(entry);
            } else {
                files.push_back(entry);
            }
        }
    }
    
    if (files.size() > kMaxNumberOfCacheFiles) {
        std::sort(files.begin(), files.end(), Comp);
        
        uint32_t files_to_remove = files.size() - kMaxNumberOfCacheFiles;
        for (uint32_t i = 0; i < files_to_remove; i++) {
            fs::remove(files[i]);
        }
    }
}

json LoadCache(const std::string& from, const std::string& to, 
    const std::string& date) {
    if (!fs::exists(kCacheDir)) {
        fs::create_directory(kCacheDir);
    }

    std::string filename = GetCacheFilename(from, to, date);
    std::ifstream file(filename);

    if (!file.is_open()) {
        return json();
    }

    json cache;
    file >> cache;
    file.close();

    return cache;
}

void CacheResults(const json& data, const std::string& from, 
            const std::string& to, const std::string& date) {
    if (!fs::exists(kCacheDir)) {
        fs::create_directory(kCacheDir);
    }

    std::string filename = GetCacheFilename(from, to, date);
    std::ofstream file(filename);
    file << data.dump(4);
    file.close();
}

Place ParsePlace(json& data) {
    Place place;
    place.code = SafeGet(data, "code");
    place.type = SafeGet(data, "type");
    place.popular_title = SafeGet(data, "popular_title");
    place.short_title = SafeGet(data, "short_title");
    place.title = SafeGet(data,"title");
    
    return place;
}

Response FindRoutes(const std::string& from, const std::string& to,
                                        const std::string& date) {
    json routes_data = GetPaths(from, to, date);
    Response response;

    if (routes_data.contains("search") && 
                    routes_data["search"].contains("date")) {
        response.search.date = routes_data["search"]["date"];
    } else {
        response.search.date = "";
    }

    response.search.from = ParsePlace(routes_data["search"]["from"]);
    response.search.to = ParsePlace(routes_data["search"]["to"]);

    for (auto segment : routes_data["segments"]) {
        
        Route* route = new Route();
                
        route->arrival = SafeGet(segment, "arrival");
        route->arrival_platform = SafeGet(segment, "arrival_platform");
        route->arrival_terminal = SafeGet(segment, "arrival_terminal");
        route->departure = SafeGet(segment, "departure");
        route->departure_platform = SafeGet(segment, "departure_platform");
        route->departure_terminal = SafeGet(segment, "departure_terminal");
        route->has_transfers = (int(segment["has_transfers"]) == 1 ? 
                                true : false);
        route->start_date = SafeGet(segment, "start_date");

        if (!route->has_transfers) {
            route->from = ParsePlace(segment["details"][0]["from"]);
            route->to = ParsePlace(segment["details"][0]["to"]);           
        } else if (segment["transfers"].size() == 1) {
            route->from = ParsePlace(segment["details"][0]["from"]);
            route->to = ParsePlace(segment["details"][2]["to"]);
            route->transfer = ParsePlace(segment["transfers"][0]);
        } else {
             continue;
        }
       
        response.routes.push_back(route);
    }
    
    return response;
}


json GetPaths(const std::string& town_from, const std::string& town_to,
                    const std::string& date) { 
    json cache = LoadCache(town_from, town_to, date);
    
    if (cache.size() > 0) {
        return cache;
    }
    
    std::string key;
    std::ifstream in(kKey);

    in >> key;
    in.close();

    std::string url = "https://api.rasp.yandex.net/v3.0/search/?apikey=" + 
        key + "&format=json&from=" + town_from + "&to=" + town_to +
        "&lang=ru_RU&page=1&date=" + date + "&transfers=true";

    cpr::Response response = cpr::Get(cpr::Url{url});

    if (response.status_code != 200) {
        std::cout << "Error request!\n";
        return json(); 
    }

    json result = json::parse(response.text);
    cache = result;
    CacheResults(cache, town_from, town_to, date);

    return result;
}

Response::~Response() {
    for (auto i : routes) {
        delete i;
    }
}