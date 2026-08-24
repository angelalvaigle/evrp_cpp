#pragma once

#include "../problem/EVRP.hpp"
#include "../solution/Solution.hpp"

class Algorithm {
public:
    virtual ~Algorithm() = default;

    virtual Solution solve(const EVRP& problem, bool type) const = 0;
};