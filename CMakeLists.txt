cmake_minimum_required(VERSION 3.5.1)

project(ginv)

set(EXEC ginv)
set(CMAKE_CXX_STANDARD 20)

include_directories(includes)
include(FetchContent)

file(GLOB SOURCES src/*.cpp)


FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(${EXEC} ${SOURCES})
target_link_libraries(${EXEC} PRIVATE nlohmann_json::nlohmann_json)