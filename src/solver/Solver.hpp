#pragma once

#include "../algorithms/Algorithm.hpp"

class Solver {
public:
    Solver(const EVRP& problem, const Algorithm& algorithm)
        : problem_(problem), algorithm_(algorithm) {}

    Solution solve() const {
        return algorithm_.solve(problem_);
    }

private:
    const EVRP& problem_;
    const Algorithm& algorithm_;
};