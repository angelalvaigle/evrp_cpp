#pragma once

#include "ProblemReader.hpp"

class SchneiderFileReader : public ProblemReader {
public:
    EVRP read_problem(
        const std::filesystem::path& path
    ) override;
};