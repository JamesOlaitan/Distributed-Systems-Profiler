cmake_minimum_required(VERSION 3.10)
project(profiler_engine)

# Specifies C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Finds required libraries
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

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
)

# Simple test for RollingPercentile using a small CTest
enable_testing()
add_executable(rp_test tests/rp_test.cpp src/data_analyzer.cpp)
target_include_directories(rp_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_test(NAME rp_test COMMAND rp_test)