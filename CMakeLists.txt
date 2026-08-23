cmake_minimum_required(VERSION 3.16)

project(evrp_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(evrp
    src/main.cpp
    src/inout/EVRPFileReader.cpp
    src/algorithms/NearestNeighborAlgorithm.cpp
    src/algorithms/GreedySearchAlgorithm.cpp
)

target_include_directories(evrp PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/inout
)