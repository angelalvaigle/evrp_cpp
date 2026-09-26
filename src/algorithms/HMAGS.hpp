#pragma once

#include "Algorithm.hpp"
#include "../solution/SolutionEvaluator.hpp"

class HMAGS : public Algorithm {
private:
    void init(const EVRP& problem, Solution pop[], Solution& best_sol, bool type) const;
    void run_HMAGS(const EVRP& problem, Solution pop[], double rank[], Solution& best_sol, bool type) const;
    void Evolution(const EVRP& problem, Solution pop[], double rank[], Solution& best_sol, bool type) const; 
    void Repopulation(const EVRP& problem, Solution pop[], double rank[], Solution& best_sol, bool type) const;
    void Selection(const EVRP& problem, Solution pop[], double rank[]) const;
    void compute_rank(Solution pop[], double rank[], int n) const;
    int choose_by_rank(double rank[], double prob) const;
    void distribute_crossover(const EVRP& problem, const Solution& parent_1, const Solution& parent_2, Solution pop[], int idx, bool type) const;
    void mutation(const EVRP& problem, Solution& solution) const;
public:
    Solution solve(const EVRP& problem, bool type) const override;

private:
    mutable SolutionEvaluator evaluator_;
};