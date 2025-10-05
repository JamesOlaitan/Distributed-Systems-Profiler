cmake_minimum_required(VERSION 3.10)
project(profiler_engine)

# Specifies C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Finds required libraries
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

# Prefer system nlohmann_json, fallback to FetchContent
find_package(nlohmann_json QUIET)
if (nlohmann_json_FOUND)
    message(STATUS "Found nlohmann_json via system package")
else()
    message(STATUS "nlohmann_json not found; fetching via FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# Adds include directories
include_directories(
    ${CURL_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Sources files
set(SOURCES
    src/main.cpp
    src/prometheus_client.cpp
    src/data_analyzer.cpp
)

add_executable(profiler_engine ${SOURCES})

target_link_libraries(profiler_engine
    ${CURL_LIBRARIES}
    Threads::Threads
    nlohmann_json::nlohmann_json
)

# Simple test for RollingPercentile using a small CTest
enable_testing()
add_executable(rp_test tests/rp_test.cpp src/data_analyzer.cpp)
target_include_directories(rp_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(rp_test PRIVATE nlohmann_json::nlohmann_json)
add_test(NAME rp_test COMMAND rp_test)