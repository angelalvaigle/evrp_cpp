cmake_minimum_required(VERSION 3.16)

project(evrp_cpp LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(evrp
    src/main.cpp
    src/problem/EVRP.cpp
    src/solution/Solution.cpp
    src/solution/SolutionEvaluator.cpp
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
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/E-n22-k4evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_e_n23_k3
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/E-n23-k3.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/E-n23-k3evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_e_n30_k3
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/E-n30-k3.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/E-n30-k3evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_e_n33_k4
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/E-n33-k4.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/E-n33-k4evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_e_n51_k5
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/E-n51-k5.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/E-n51-k5evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_e_n76_k7
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/E-n76-k7.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/E-n76-k7evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_e_n101_k8
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/E-n101-k8.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/E-n101-k8evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_x_n143_k7
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/X-n143-k7.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/X-n143-k7evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_x_n214_k11
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/X-n214-k11.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/X-n214-k11evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_x_n351_k40
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/X-n351-k40.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/X-n351-k40evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_x_n459_k26
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/X-n459-k26.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/X-n459-k26evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_x_n573_k30
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/X-n573-k30.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/X-n573-k30evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_x_n685_k75
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/X-n685-k75.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/X-n685-k75evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_x_n749_k98
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/X-n749-k98.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/X-n749-k98evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_x_n819_k171
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/X-n819-k171.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/X-n819-k171evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_x_n916_k207
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/X-n916-k207.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/X-n916-k207evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

add_test(
    NAME greedy_search_x_n1001_k43
    COMMAND ${CMAKE_COMMAND}
        -DTEST_BINARY=$<TARGET_FILE:evrp>
        -DTEST_ALGORITHM=GS
        -DTEST_INSTANCE=${PROJECT_SOURCE_DIR}/data/X-n1001-k43.evrp
        -DTEST_EXPECTED_DIR=${PROJECT_SOURCE_DIR}/test/solutions/GS/X-n1001-k43evrp
        -P ${PROJECT_SOURCE_DIR}/test/compare_runs.cmake
)

