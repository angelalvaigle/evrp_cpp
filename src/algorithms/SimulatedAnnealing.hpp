#pragma once

#include <limits>

#include "Algorithm.hpp"
#include "../solver/SolverParameters.hpp"

struct SimulatedAnnealingParameters {
    SolverParameters solver;
    double alpha = 1000.0;
    double beta = 0.333333;
    double t_current = 1.0;
    double t_cool = 0.9999;
    double t_end = 0.02;
    double t_greedy = 10.0;
    double t_v_factor = 10.0;
    double optimal_fitness = std::numeric_limits<double>::max();
};

class SimulatedAnnealing : public Algorithm {
public:
    explicit SimulatedAnnealing(
        SimulatedAnnealingParameters parameters = {}
    ) : parameters_(parameters) {}

    Solution solve(const EVRP& problem, bool type) const override;

private:
    // TODO: Implement the first neighborhood move.
    void greedy_1(Solution& solution) const;

    // TODO: Implement the second neighborhood move.
    void greedy_2(Solution& solution) const;

    // TODO: Implement solution setup/decoding.
    void setup(Solution& solution) const;

    SimulatedAnnealingParameters parameters_;
    // void redistribute_customer(const EVRP& problem, Solution& solution) const;
    // void set_tour_index(Solution& solution) const;
    // double get_capacity_of_tour(const EVRP& problem, const Solution& solution, int tour_id) const;
};