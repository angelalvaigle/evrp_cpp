#pragma once

#include <filesystem>
#include <string>

#include "../problem/EVRP.hpp"
#include "../solution/Solution.hpp"

class SolutionFileWriter {
public:
    void write_solution(
        const std::filesystem::path& output_directory,
        const std::string& algorithm,
        const std::string& instance_name,
        int run,
        const EVRP& problem,
        const Solution& solution
    ) const;
};