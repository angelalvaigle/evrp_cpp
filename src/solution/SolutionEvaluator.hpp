#pragma once

class EVRP;
class Solution;

class SolutionEvaluator {
public:
    double fitness_evaluation(const EVRP& problem, const Solution& solution) const;
    double distance(const EVRP& problem, int from, int to) const;
    void reset() const;
    void full_evaluation() const;
    void partial_evaluation(int problem_size) const;
    double get_evals() const;
    void add_penalty(Solution& solution) const;

private:
    mutable double evals_ = 0.0;
};