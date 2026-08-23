// main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>

#include "algorithms/NearestNeighborAlgorithm.hpp"
#include "algorithms/GreedySearchAlgorithm.hpp"
#include "inout/EVRPFileReader.hpp"
#include "solver/Solver.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Please specify a problem instance\n";
        return 0;
    }

    EVRPFileReader reader;
    const EVRP problem = reader.read_problem(argv[1]);
    GreedySearchAlgorithm algorithm;
    Solver solver(problem, algorithm);
    const Solution solution = solver.solve();

    std::cout << problem.problem_instance << ": "
              << solution.num_of_tours << " rutas\n";
    for (int route_id = 0; route_id < solution.num_of_tours; ++route_id) {
        const Segment& tour = solution.tours[route_id];
        std::cout << "Ruta " << route_id + 1 << ": " << problem.DEPOT;
        for (int index = tour.left; index <= tour.right; ++index) {
            std::cout << ' ' << solution.order[index];
        }
        std::cout << ' ' << problem.DEPOT;
        std::cout << '\n';
    }
    return 0;
}