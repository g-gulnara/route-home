add_library(way STATIC way.cpp way.h)

target_link_libraries(way PUBLIC cpr::cpr)
target_link_libraries(way PUBLIC nlohmann_json::nlohmann_json)
