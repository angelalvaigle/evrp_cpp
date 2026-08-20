// EVRPFileReader.hpp
#pragma once

#include "ProblemReader.hpp"

class EVRPFileReader : public ProblemReader {
public:
    EVRP read_problem(
        const std::filesystem::path& path
    ) override;
};
