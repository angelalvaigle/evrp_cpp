#include "SolutionEvaluator.hpp"

#include "Solution.hpp"
#include "../problem/EVRP.hpp"

#include "../solver/SolverParameters.hpp"

double SolutionEvaluator::fitness_evaluation(
    const EVRP& problem,
    const Solution& solution
) const {
    // orig: /****************************************************************/
    // orig: /* Returns the solution quality of the solution. Taken as input */
    // orig: /* an array of node indeces and its length                      */
    // orig: /****************************************************************/
    // orig: the format of the solution that this method evaluates is the following
    // orig: Node id:  0 - 5 - 6 - 8 - 0 - 1 - 2 - 3 - 4 - 0 - 7 - 0
    // orig: Route id: 1 - 1 - 1 - 1 - 2 - 2 - 2 - 2 - 2 - 3 - 3 - 3
    // orig: this solution consists of three routes:
    // orig: Route 1: 0 - 5 - 6 - 8 - 0
    // orig: Route 2: 0 - 1 - 2 - 3 - 4 - 0
    // orig: Route 3: 0 - 7 - 0
    return solution.get_total_distance(problem);
}

void SolutionEvaluator::add_penalty(Solution& solution) const {
    // orig: add penalty for false indv
    solution.fitness *= SolverParameters::PENALTY;
}