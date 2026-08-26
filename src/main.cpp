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
        std::cout << "Usage: " << argv[0] << " <GS|SA> <problem-instance>\n";
        return 1;
    }

    const std::string algorithm_name = argv[1];

    EVRPFileReader reader;
    const EVRP problem = reader.read_problem(argv[2]);

    constexpr int total_runs = 10;
    const std::string instance_name = std::filesystem::path(argv[2]).filename().string();
    SolutionFileWriter writer;

    for (int run = 1; run <= total_runs; ++run) {
        start_run(run);

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

        writer.write_solution(
            "output_files",
            algorithm_name,
            instance_name,
            run,
            problem,
            solution
        );

        std::cout << problem.problem_instance << " run " << run << ": "
                  << solution.num_of_tours << " rutas\n";
        const auto node_id = [&problem](int index) -> const std::string& {
            return problem.node_list.at(index).id;
        };
        for (const int node_index : solution.solution) {
            std::cout << node_id(node_index) << " ";
        }
        std::cout << "\n";
    }

    std::cout << "Se completaron " << total_runs << " ejecuciones.\n";
    return 0;
}
