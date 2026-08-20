// main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>

#include "inout/EVRPFileReader.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Please specify a problem instance\n";
        return 0;
    }

    EVRPFileReader reader;
    const EVRP problem = reader.read_problem(argv[1]);
    std::cout << problem.problem_instance << ": "
              << problem.problem_size << " nodes, "
              << problem.NUM_OF_STATIONS << " stations\n";
    return 0;
}