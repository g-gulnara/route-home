cmake_minimum_required(VERSION 3.5.0)
project(labwork6)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)

FetchContent_Declare(
  cpr
  GIT_REPOSITORY https://github.com/libcpr/cpr.git
)

FetchContent_MakeAvailable(cpr)

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)

FetchContent_MakeAvailable(json)

set(CPR_BUILD_TESTS OFF CACHE BOOL "Disable CPR tests" FORCE)

add_subdirectory(bin)
add_subdirectory(lib)
