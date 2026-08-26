cmake_minimum_required(VERSION 3.16)

project(evrp_cpp LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(evrp
    src/main.cpp
    src/solution/Solution.cpp
    src/inout/EVRPFileReader.cpp
    src/inout/SolutionFileWriter.cpp
    src/algorithms/GreedySearch.cpp
    src/algorithms/SimulatedAnnealing.cpp
)

target_include_directories(evrp PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/inout
)

add_test(
    NAME greedy_search_e_n22_k4
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/E-n22-k4.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/E-n22-k4.evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)