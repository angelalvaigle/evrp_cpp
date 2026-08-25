#include "SimulatedAnnealing.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>

namespace {

    double get_distance(const EVRP& problem, int from, int to) {
        const node& origin = problem.node_list.at(from);
        const node& destination = problem.node_list.at(to);
        const double x = origin.x - destination.x;
        const double y = origin.y - destination.y;
        return std::sqrt(x * x + y * y);
    }

    // bool is_customer(const EVRP& problem, int node_id) {
    //     return node_id != problem.DEPOT && !problem.charging_station.at(node_id);
    // }

    std::vector<std::vector<int>> compute_nearest_points(const EVRP& problem) {
        std::vector<std::vector<int>> nearest(problem.NUM_OF_CUSTOMERS + 1);

        for(int i = 1; i <= problem.NUM_OF_CUSTOMERS; i++) {
            nearest[i].assign(problem.NUM_OF_CUSTOMERS, 0);
            for(int j = 0; j < problem.NUM_OF_CUSTOMERS; j++) {
                nearest[i][j] = j + 1;
            }

            std::sort(nearest[i].begin(), nearest[i].end(), [&, i](int j, int k) {
                return get_distance(problem, i, j) < get_distance(problem, i, k);
            });
        }
        return nearest;
    }
}

Solution SimulatedAnnealing::solve(const EVRP& problem, bool type) const {
    // if (problem.node_list.empty() || problem.DEPOT < 0 ||
    //     problem.DEPOT >= problem.ACTUAL_PROBLEM_SIZE) {
    //     throw std::invalid_argument("La instancia EVRP no tiene un deposito valido");
    // }

    Solution solution;
    Solution new_solution;
    Solution best_solution;
    solution.order.resize(problem.NUM_OF_CUSTOMERS);
    solution.index_of_customer.assign(problem.NUM_OF_CUSTOMERS + 1, 0);
    solution.tour_index.assign(problem.NUM_OF_CUSTOMERS + 1, 0);
    
    double t_current = parameters_.t_current;
    double t_cool = parameters_.t_cool;
    double t_end = parameters_.t_end;
    double t_greedy = parameters_.t_greedy;
    double alpha = parameters_.alpha;
    double beta = parameters_.beta;

    double improve;

    // cout << "initial fitness: " << solution.get_fitness() << "\n";

    // int t_v = (int) (ACTUAL_PROBLEM_SIZE * this->t_v_factor);
    int cnt_div = 0;
    
    int G = 0;
    std::size_t evaluations = 0;
    
    double sqrt_n = std::log10(problem.ACTUAL_PROBLEM_SIZE);
    std::vector<double> conv;
    const std::size_t termination =
        parameters_.solver.termination_factor *
        static_cast<std::size_t>(problem.ACTUAL_PROBLEM_SIZE);

    while (evaluations < termination && t_current > t_end){
        t_greedy = problem.ACTUAL_PROBLEM_SIZE * beta;
        // double prob = (double)rand() / RAND_MAX;
        t_cool = (alpha * sqrt_n - 1.0) / (alpha * sqrt_n);
        do {
            // cout << "Search: " << t_current << "\n";
            conv.push_back(solution.get_fitness());
            new_solution = solution;
            
            double rand_t = (double) rand() / (double) RAND_MAX;
            if(rand_t <= 0.5){
                greedy_1(new_solution);
            } else {
                greedy_2(new_solution);
            }
            setup(new_solution);
            ++evaluations;
            improve = solution.get_fitness() - new_solution.get_fitness();
            G++;

            if (improve > 0)
                break;

            if(new_solution.get_fitness() + 1e10 > SolverParameters::INF) {
                continue;
            }

            /* Termination */
            if (G >= t_greedy) {
                double upper = abs(new_solution.get_fitness() - solution.get_fitness() ) / 
                    abs(new_solution.get_fitness() - best_solution.get_fitness() + 1e-5);
                // double rho = exp(upper) * t_current;
                double accept_prob = exp(- upper / t_current);
                // cout << upper << " " << solution.get_fitness() << " " << new_solution.get_fitness() << " "
                    // << best_solution.get_fitness() << " prob: " << accept_prob << " " << t_current << "\n";
                // getchar();
                double r = ((double) rand() / (RAND_MAX));
                if(accept_prob > r){
                    solution = new_solution;
                }
                
                /* Compulsive Accept */
                cnt_div ++;
                break;
            }
            
        } while (improve < 0 && evaluations < termination);
    
        solution = new_solution;
        if (solution.is_valid_solution(problem) &&
            solution.fitness < best_solution.fitness) {
            best_solution = solution;
        }
        t_current *= t_cool;
        t_current = std::max(t_current, parameters_.t_end);

        G = 0;
    }//end while
    // save_conv(conv, "conv_file_3");

    return best_solution;
}

void SimulatedAnnealing::greedy_1(Solution& solution) const {
    // TODO: Implement the first neighborhood move.
}

void SimulatedAnnealing::greedy_2(Solution& solution) const {
    // TODO: Implement the second neighborhood move.
}

void SimulatedAnnealing::setup(Solution& solution) const {
    // TODO: Implement solution setup/decoding.
}