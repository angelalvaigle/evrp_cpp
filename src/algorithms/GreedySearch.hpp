#pragma once

#include "Algorithm.hpp"

class GreedySearch : public Algorithm {
private:
    void redistribute_customer(const EVRP& problem, Solution& solution) const;
    void setup(const EVRP& problem, Solution& solution, bool type) const;
    void local_search(const EVRP& problem, Solution& solution) const;
    void complete_gen(const EVRP& problem, Solution& solution, bool type) const;
    bool complete_subgen(const EVRP& problem, Solution& solution,
                         int* full_path, int* gen_temp, int left, int right,
                         int& count, bool type) const;
    void optimize_station(const EVRP& problem, int* full_path, int left, int right,
                          const std::vector<int>& remaining_energy, bool type) const;
public:
    Solution solve(const EVRP& problem, bool type) const override;
};