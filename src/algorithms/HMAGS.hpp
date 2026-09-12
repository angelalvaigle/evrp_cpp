#pragma once

#include "Algorithm.hpp"
#include "../solution/SolutionEvaluator.hpp"

class HMAGS : public Algorithm {
private:
    void opt_generate(const EVRP& problem, Solution& solution, bool type) const;
    void redistribute_customer(const EVRP& problem, Solution& solution) const;
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
    void init(const EVRP& problem, Solution pop[], Solution& best_sol, bool type) const;
    void run_HMAGS(const EVRP& problem, Solution pop[], double rank[], Solution& best_sol, bool type) const;
    void Evolution(const EVRP& problem, Solution pop[], double rank[], Solution& best_sol, bool type) const; 
    void Repopulation(const EVRP& problem, Solution pop[], double rank[], Solution& best_sol, bool type) const;
    void Selection(const EVRP& problem, Solution pop[], double rank[]) const;
    void compute_rank(Solution pop[], double rank[], int n) const;
    int choose_by_rank(double rank[], double prob) const;
    void distribute_crossover(const EVRP& problem, Solution parent_1, Solution parent_2, Solution pop[], int idx, bool type) const;
    void mutation(const EVRP& problem, Solution& solution) const;
public:
    Solution solve(const EVRP& problem, bool type) const override;

private:
    mutable SolutionEvaluator evaluator_;
};