#pragma once

class EVRP;
class Solution;

class SolutionEvaluator {
public:
    double fitness_evaluation(const EVRP& problem, const Solution& solution) const;
    void add_penalty(Solution& solution) const;
};