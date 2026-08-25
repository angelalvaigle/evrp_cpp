// main.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <cstdlib>
#include <string>
#include <unordered_map>

#include "algorithms/GreedySearch.hpp"
#include "algorithms/SimulatedAnnealing.hpp"
#include "inout/EVRPFileReader.hpp"
#include "inout/SolutionFileWriter.hpp"
#include "solver/Solver.hpp"

void start_run(int run) {
    std::srand(run);
    std::cout << "Run: " << run << " with random seed " << run << '\n';
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <GS|SA> <problem-instance> [run]\n";
        return 1;
    }

    const std::string algorithm_name = argv[1];
    const int run = argc >= 4 ? std::atoi(argv[3]) : 1;
    start_run(run);

    EVRPFileReader reader;
    const EVRP problem = reader.read_problem(argv[2]);
    GreedySearch greedy_search;
    SimulatedAnnealing simulated_annealing;

    const std::unordered_map<std::string, const Algorithm*> algorithms{
        {"GS", &greedy_search},
        {"SA", &simulated_annealing},
    };
    const auto algorithm = algorithms.find(algorithm_name);
    if (algorithm == algorithms.end()) {
        std::cerr << "Unknown algorithm: " << algorithm_name << ". Use GS or SA.\n";
        return 1;
    }

    Solver solver(problem, *algorithm->second);
    const Solution solution = solver.solve(true);

    SolutionFileWriter writer;
    writer.write_solution(
        "output_files",
        algorithm_name,
        std::filesystem::path(argv[2]).filename().string(),
        run,
        problem,
        solution
    );

    const auto node_id = [&problem](int index) -> const std::string& {
        return problem.node_list.at(index).id;
    };

    std::cout << problem.problem_instance << ": "
              << solution.num_of_tours << " rutas\n";
    std::cout << "Solucion: ";
    for (const int node_index : solution.solution) {
        std::cout << node_id(node_index) << ' ';
    }
    std::cout << "\nFichero: output_files/" << run << "/solution_"
              << algorithm_name << '_' << std::filesystem::path(argv[2]).filename().string()
              << ".txt\n";
    return 0;
}