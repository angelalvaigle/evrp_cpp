#include "SolutionFileWriter.hpp"

#include <fstream>
#include <stdexcept>

void SolutionFileWriter::write_solution(
    const std::filesystem::path& output_directory,
    const std::string& algorithm,
    const std::string& instance_name,
    int run,
    const EVRP& problem,
    const Solution& solution
) const {
    const std::filesystem::path run_directory = output_directory / std::to_string(run);
    std::filesystem::create_directories(run_directory);

    const std::filesystem::path output_file =
        run_directory / ("solution_" + algorithm + "_" + instance_name + ".txt");
    std::ofstream file(output_file);
    if (!file) {
        throw std::runtime_error("Unable to create solution file: " + output_file.string());
    }

    file << problem.ACTUAL_PROBLEM_SIZE << '\n';
    for (int index = 0; index < problem.ACTUAL_PROBLEM_SIZE; ++index) {
        const int type = index == problem.DEPOT
            ? 0
            : (index < problem.problem_size ? 1 : 2);
        const node& current = problem.node_list.at(index);
        file << type << ' ' << current.x << ' ' << current.y << '\n';
    }

    file << solution.steps << '\n';
    for (const int index : solution.solution) {
        const node& current = problem.node_list.at(index);
        file << current.x << ' ' << current.y << '\n';
    }
}