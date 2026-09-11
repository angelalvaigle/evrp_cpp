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
    void greedy_1(const EVRP& problem, Solution& solution) const;

    void greedy_2(const EVRP& problem, Solution& solution) const;

    void setup(const EVRP& problem, Solution& solution, bool type) const;
    void local_search(const EVRP& problem, Solution& solution) const;
    void complete_gen(const EVRP& problem, Solution& solution, bool type) const;
    bool complete_subgen(const EVRP& problem, Solution& solution,
                         int* full_path, int* gen_temp, int left, int right,
                         int& count, bool type) const;
    void optimize_station(const EVRP& problem, int* full_path, int left, int right,
                          const std::vector<int>& remaining_energy, bool type) const;
    int nearest_station(const EVRP& problem, int from, int to, double energy) const;
    int nearest_station_back(const EVRP& problem, int from, int to, double energy) const;

    SimulatedAnnealingParameters parameters_;
    // void redistribute_customer(const EVRP& problem, Solution& solution) const;
    // void set_tour_index(Solution& solution) const;
    // double get_capacity_of_tour(const EVRP& problem, const Solution& solution, int tour_id) const;
};