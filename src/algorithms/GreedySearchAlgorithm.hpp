#pragma once

#include "Algorithm.hpp"

class GreedySearchAlgorithm : public Algorithm {
private:
    void redistribute_customer(const EVRP& problem, Solution& solution) const;
    void set_tour_index(Solution& solution) const;
    double get_capacity_of_tour(const EVRP& problem, const Solution& solution, int tour_id) const;
public:
    Solution solve(const EVRP& problem, bool type) const override;
};