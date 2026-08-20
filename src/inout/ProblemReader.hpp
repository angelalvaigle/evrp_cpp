// ProblemReader.hpp
#pragma once

#include <filesystem>

#include "../problem/EVRP.hpp"

class ProblemReader {
public:
    virtual ~ProblemReader() = default;

    virtual EVRP read_problem(
        const std::filesystem::path& path
    ) = 0;
};
