#pragma once

#include <vector>

#include "../solver/SolverParameters.hpp"

class EVRP;

struct Segment {
    int left = 0;
    int right = -1;
};

class Solution {
public:
    // Genetic representation: the customers, grouped by tour ranges.
    std::vector<int> order;
    std::vector<Segment> tours;
    std::vector<int> tour_index;
    std::vector<int> index_of_customer;
    int num_of_tours = 0;

    // Decoded representation used for validation and evaluation.
    std::vector<int> solution;
    int steps = 0;
    double fitness = SolverParameters::INF;

    // Solution fitness accessor
    double get_fitness() const;

    // Solution validation
    bool check_solution(const EVRP& problem) const;
    bool is_valid_solution(const EVRP& problem) const;

    // Steps accessor
    int get_steps() const;
};