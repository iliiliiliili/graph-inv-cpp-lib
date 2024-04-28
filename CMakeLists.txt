cmake_minimum_required(VERSION 3.9)

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

#------------test----------------

enable_testing()

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

set(TEST_EXEC test_ginv)
file(GLOB TEST_SOURCES tests/*.cpp)
add_executable(${TEST_EXEC} ${TEST_SOURCES})
target_link_libraries(${TEST_EXEC} nlohmann_json::nlohmann_json GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${TEST_EXEC})

#------------end-test------------