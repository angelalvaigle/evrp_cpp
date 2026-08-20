#pragma once

#include "Algorithm.hpp"

class NearestNeighborAlgorithm : public Algorithm {
public:
    Solution solve(const EVRP& problem) const override;
};