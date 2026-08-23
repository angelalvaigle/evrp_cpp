#pragma once

#include "Algorithm.hpp"

class GreedySearchAlgorithm : public Algorithm {
public:
    Solution solve(const EVRP& problem) const override;
};