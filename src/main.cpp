// main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>

#include "algorithms/NearestNeighborAlgorithm.hpp"
#include "inout/EVRPFileReader.hpp"
#include "solver/Solver.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Please specify a problem instance\n";
        return 0;
    }

    EVRPFileReader reader;
    const EVRP problem = reader.read_problem(argv[1]);
    NearestNeighborAlgorithm algorithm;
    Solver solver(problem, algorithm);
    const Solution solution = solver.solve();

    std::cout << problem.problem_instance << ": "
              << solution.routes.size() << " rutas, distancia total "
              << solution.total_distance << "\n";
    for (std::size_t route_id = 0; route_id < solution.routes.size(); ++route_id) {
        std::cout << "Ruta " << route_id + 1 << ":";
        for (int node_id : solution.routes[route_id]) {
            std::cout << ' ' << node_id + 1;
        }
        std::cout << '\n';
    }
    return 0;
}