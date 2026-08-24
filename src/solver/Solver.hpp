#pragma once

#include "../algorithms/Algorithm.hpp"

class Solver {
public:
    Solver(const EVRP& problem, const Algorithm& algorithm)
        : problem_(problem), algorithm_(algorithm) {}

    Solution solve(bool type = false) const {
        return algorithm_.solve(problem_, type);
    }

private:
    const EVRP& problem_;
    const Algorithm& algorithm_;
};