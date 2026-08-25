#pragma once

#include <cstddef>

struct SolverParameters {
    static constexpr double INF = 2e15;
    static constexpr double PENALTY = 1.3;
    static constexpr int MAX_NUM_FINDING_SAFE = 10;
    std::size_t termination_factor = 25000;
};
