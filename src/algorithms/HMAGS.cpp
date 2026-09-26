#include "HMAGS.hpp"
#include "GreedySearch.hpp"
#include "../solution/SolutionEvaluator.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>


const int NUM_OF_INDVS = 200;
const double PR_MUTATE = 0.1;
// const int MAX_NODE = 1500;

// int hmags_remaining_energy[MAX_NODE];
// int hmags_gen_temp[MAX_NODE];
// int hmags_full_path[MAX_NODE];
// short int hmags_path[MAX_NODE];

Solution HMAGS::solve(const EVRP& problem, bool type) const {
    problem.reset_nearest_points();
    problem.reset_evaluations();
    Solution best_sol;
    best_sol.set_fitness(SolverParameters::INF);
    Solution pop[3 * NUM_OF_INDVS];
    double rank[3 * NUM_OF_INDVS];

    init(problem, pop, best_sol, type);

    const double termination =
        25000.0 * static_cast<double>(problem.ACTUAL_PROBLEM_SIZE);
    while (problem.get_evaluations() < termination) {
        run_HMAGS(problem, pop, rank, best_sol, type); 
    }

    std::cout << "HMAGS evaluations: " << problem.get_evaluations() << '\n';
    return best_sol;
}

void HMAGS::init(const EVRP& problem, Solution pop[], Solution& best_sol, bool type) const {
    GreedySearch greedy;
    for(int i = 0; i < NUM_OF_INDVS; i++) {
        pop[i] = greedy.solve(problem, type);
        if (pop[i].get_fitness() < best_sol.get_fitness()){
            best_sol = pop[i];
        }
    }
}

// bool is_customer(const EVRP& problem, int node_id) {
//     return node_id != problem.DEPOT && !problem.charging_station.at(node_id);
// }

/*implement your heuristic in this function*/
void HMAGS::run_HMAGS(const EVRP& problem, Solution pop[], double rank[], Solution& best_sol, bool type) const {
    Evolution(problem, pop, rank, best_sol, type);
}

void HMAGS::Evolution(const EVRP& problem, Solution pop[], double rank[], Solution& best_sol, bool type)  const {
    Repopulation(problem, pop, rank, best_sol, type);
    Selection(problem, pop, rank);
}

void HMAGS::Repopulation(const EVRP& problem, Solution pop[], double rank[], Solution& best_sol, bool type) const {
    compute_rank(pop, rank, NUM_OF_INDVS);
    for(int i = 0; i < 2 * NUM_OF_INDVS; i += 2) {
        // choose the two parents
        double p1 = (double) rand() / (double) RAND_MAX;
        double p2 = (double) rand() / (double) RAND_MAX;
        int idx_1 = choose_by_rank(rank, p1);
        int idx_2 = choose_by_rank(rank, p2);
        while(p1 == p2)
            p2 = (double) rand() / (double) RAND_MAX;
        distribute_crossover(problem, pop[idx_1], pop[idx_2], pop, NUM_OF_INDVS + i, type);
    }
    int cnt = 0;
    for (int i = NUM_OF_INDVS; i < 3 * NUM_OF_INDVS; i++){
        if( pop[i].get_steps() == 0){
            cnt++;
        }
        if (pop[i].get_fitness() < best_sol.get_fitness()){
            best_sol = pop[i];
        }
    }
}

void HMAGS::compute_rank(Solution pop[], double rank[], int n) const {
    double sum = 0;
    double fit_min = SolverParameters::INF;
    double fit_max = 0;
    for(int i = 0; i < n; i++){
        fit_min = std::min(fit_min, pop[i].get_fitness());
        fit_max = std::max(fit_max, pop[i].get_fitness());
    }
    for(int i = 0; i < n; i++){
        double temp_fit = std::pow((fit_max - pop[i].get_fitness()) / (fit_max - fit_min + 1e-6), 2);
        sum += temp_fit;
        rank[i] = temp_fit;
    }
    for(int i = 0; i < n; i++){
        rank[i] /= sum;
        if(i > 0)
            rank[i] += rank[i - 1];
    }
}

int HMAGS::choose_by_rank(double rank[], double prob) const{
    return (int) (std::upper_bound(rank, rank + NUM_OF_INDVS, prob) - rank);
}

void HMAGS::distribute_crossover(const EVRP& problem, const Solution& parent_1, const Solution& parent_2, Solution pop[], int idx, bool type) const {
    int num = rand()%(problem.NUM_OF_CUSTOMERS) + 1;// so ngau nhien tu 1 den size of customers
    int id1 = parent_1.tour_index[num];// customer 'num' of id1 tour in parent_1
    int id2 = parent_2.tour_index[num];// customer 'num' of id2 tour in parent_2
    int have[problem.NUM_OF_CUSTOMERS + 1];
    int alens[problem.NUM_OF_CUSTOMERS + 1];

    Solution child1;
    Solution child2;
    // child1 = parent_1;   // child1.copy_order(parent_1);
    // child2 = parent_2;   // child2.copy_order(parent_2);

    child1.copy_order(parent_1);
    child2.copy_order(parent_2);

    for(int i = 0; i <= problem.NUM_OF_CUSTOMERS; i++){
        have[i] = 0;// have : exists in the alens or not
        alens[i] = 0;
    }

    int index = 0;

    // merge tour_id1, tour_id2 into alens
    for(int i = parent_1.tours[id1].right; i >= parent_1.tours[id1].left; i--) {
        alens[index++] = parent_1.order[i];
        have[parent_1.order[i]] = 1;
    }

    for(int i = parent_2.tours[id2].right; i >= parent_2.tours[id2].left; i--) {
        if(!have[parent_2.order[i]]) {
            alens[index++] = parent_2.order[i];
            have[parent_2.order[i]] = 1;
        }
    }
    int index2 = 0; // size of alens

    // Distribute alens to both of child1 and child2

    for(int i = 0; i < problem.NUM_OF_CUSTOMERS; i++){
        if(have[child1.order[i]]){
            child1.order[i] = alens[--index];
        }
        if(have[child2.order[i]]){
            child2.order[i] = alens[index2++];
        }
    }
    mutation(problem, child1);
    mutation(problem, child2);
    pop[idx].copy_order(child1);
    pop[idx + 1].copy_order(child2);
    GreedySearch greedy;
    greedy.setup(problem, pop[idx], type);
    greedy.setup(problem, pop[idx + 1], type);
}

void HMAGS::Selection(const EVRP& problem, Solution pop[], double rank[]) const {

    std::sort(pop, pop + 3 * NUM_OF_INDVS, [](const Solution& x, const Solution& y) {
        return x.get_fitness() < y.get_fitness();
    });

    compute_rank(pop, rank, 2 * NUM_OF_INDVS);

    for (int i = 0; i < NUM_OF_INDVS; i++) {
        double prob = (double) rand() / (double) RAND_MAX;
        int idx = choose_by_rank(rank, prob);
        pop[2 * NUM_OF_INDVS + i].copy_order(pop[idx]);
    }
    
    for (int i = 0; i < NUM_OF_INDVS; i++) {
        pop[i].copy_order(pop[NUM_OF_INDVS * 2 + i]);
    }
    
}

void HMAGS::mutation(const EVRP& problem, Solution& solution) const {
    const auto& nearest = problem.get_nearest_points();
    double mutate_prob_1 = (double) rand() / (double) RAND_MAX;
    double mutate_prob_2 = (double) rand() / (double) RAND_MAX;
    solution.set_tour_index();
    if(mutate_prob_1 < PR_MUTATE){
        int customer = rand() % (problem.NUM_OF_CUSTOMERS) + 1;
        static int near_customer;
        near_customer = -1;

        for(int x: nearest[customer]){
            if(solution.tour_index[x] != solution.tour_index[customer]){
                near_customer = x;
                break;
            }
        }

        if(near_customer != -1){

            for(int i = solution.tours[solution.tour_index[customer]].left; i <= solution.tours[solution.tour_index[customer]].right; i++){
                if(solution.order[i] == customer){
                    solution.order[i] = near_customer;
                    break;
                }
            }
            
            for(int i = solution.tours[solution.tour_index[near_customer]].left; i <= solution.tours[solution.tour_index[near_customer]].right; i++){
                if(solution.order[i] == near_customer){
                    solution.order[i] = customer;
                    break;
                }
            }

            std::swap(solution.tour_index[customer], solution.tour_index[near_customer]);
        }
        return;
    }

    if(mutate_prob_2 < PR_MUTATE){
        // choose randomly a index of order
        int customer_index = rand() % (problem.NUM_OF_CUSTOMERS);
        int customer = solution.order[customer_index];
        double cost = solution.get_capacity_of_tour(
            problem, solution.tour_index[customer]);

        int near_customer = -1;
        for(int x: nearest[customer]){
            if(solution.tour_index[x] != solution.tour_index[customer] && cost + problem.get_customer_demand(x) <= problem.MAX_CAPACITY
                    && solution.tours[solution.tour_index[x]].right - solution.tours[solution.tour_index[x]].left > 1){

                near_customer = x;
                break;
            }
        }
        if(near_customer != -1){
            int near_customer_tour_index = solution.tour_index[near_customer], customer_tour_index = solution.tour_index[customer];

            int near_customer_index = -1;
            for(int i = solution.tours[near_customer_tour_index].left; i <= solution.tours[near_customer_tour_index].right; i++){
                if(solution.order[i] == near_customer){
                    near_customer_index = i;
                    break;
                }
            }

            // pick from near_customer_tour_index . customer_tour_index
            if(customer_tour_index < near_customer_tour_index){
                std::swap(solution.order[near_customer_index], solution.order[solution.tours[near_customer_tour_index].left]);
                solution.tours[near_customer_tour_index].left++;
                for(int i = near_customer_tour_index - 1; i > customer_tour_index; i--){
                    for(int j = solution.tours[i].right; j >= solution.tours[i].left; j--){
                        solution.order[j + 1] = solution.order[j];
                    }
                    solution.tours[i].left++;
                    solution.tours[i].right++;
                }
                solution.tours[customer_tour_index].right++;
                solution.order[solution.tours[customer_tour_index].right] = near_customer;
                solution.tour_index[near_customer] = customer_tour_index;
            } else{
                std::swap(solution.order[near_customer_index], solution.order[solution.tours[near_customer_tour_index].right]);
                solution.tours[near_customer_tour_index].right--;
                for(int i = near_customer_tour_index + 1; i <= customer_tour_index; i++){
                    for(int j = solution.tours[i].left; j <= solution.tours[i].right; j++){
                        solution.order[j - 1] = solution.order[j];
                    }
                    solution.tours[i].left--;
                    solution.tours[i].right--;
                }
                solution.tours[customer_tour_index].right++;
                solution.order[solution.tours[customer_tour_index].right] = near_customer;
                solution.tour_index[near_customer] = customer_tour_index;
            }
        }
        return;
    }
    // if(mutate_prob_3 < PR_MUTATE){
    //     int customer_index = rand() % (NUM_OF_CUSTOMERS);
    //     int near_customer_index = rand() % (NUM_OF_CUSTOMERS);
    //     int customer = solution.order[customer_index];
    //     int near_customer = solution.order[near_customer_index];
    //     swap(solution.order[near_customer_index], solution.order[customer_index]);
    //     swap(solution.tour_index[near_customer], solution.tour_index[customer]);
    // }
}