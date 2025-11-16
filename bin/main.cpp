#include <fstream>
#include <cpr/cpr.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include "lib/way.h"

using json = nlohmann::json;

const std::string saint_petersburg_code = "c2";
const std::string naberezhnye_chelny_code = "c236";

int main(int argc, char** argv) {
    SetConsoleOutputCP(CP_UTF8);

    std::string parsing_arguments_result = ParseArguments(argc, argv);
    if (parsing_arguments_result == "") {
        std::cerr << "Error parsing arguments!\nUsage: YYYY-MM-DD";
        return 1;
    }
    Process(parsing_arguments_result,saint_petersburg_code, 
                                            naberezhnye_chelny_code);    

}