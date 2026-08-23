#include "GreedySearchAlgorithm.hpp"

#include <algorithm>
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

    bool is_customer(const EVRP& problem, int node_id) {
        return node_id != problem.DEPOT && !problem.charging_station.at(node_id);
    }

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

Solution GreedySearchAlgorithm::solve(const EVRP& problem) const {
    // if (problem.node_list.empty() || problem.DEPOT < 0 ||
    //     problem.DEPOT >= problem.ACTUAL_PROBLEM_SIZE) {
    //     throw std::invalid_argument("La instancia EVRP no tiene un deposito valido");
    // }

    Solution solution;
    solution.order.resize(problem.NUM_OF_CUSTOMERS);
    solution.index_of_customer.assign(problem.NUM_OF_CUSTOMERS + 1, 0);
    const std::vector<std::vector<int>> nearest = compute_nearest_points(problem);
    std::vector<int> have(problem.NUM_OF_CUSTOMERS + 1, 0);
    for(int i = 0; i < problem.NUM_OF_CUSTOMERS; i++) {
        solution.order[i] = i + 1;
        solution.index_of_customer[i + 1] = i;
    }
    // for(int i = 0; i < NUM_OF_CUSTOMERS; i++) {
    //     int idx_1, idx_2;
    //     idx_1 = rand() % NUM_OF_CUSTOMERS;
    //     idx_2 = rand() % NUM_OF_CUSTOMERS;
    //     swap(index_of_customer[order[idx_1]], index_of_customer[order[idx_2]]);
    //     swap(order[idx_1], order[idx_2]);
    // }
    int first_customer_index, capacity, idx;
    idx = 0;
    while(idx < problem.NUM_OF_CUSTOMERS) {
        // rand()%N return random number from 0 to N-1
        first_customer_index = rand()%(problem.NUM_OF_CUSTOMERS - idx) + idx;
        solution.index_of_customer[solution.order[idx]] = first_customer_index;
        std::swap(solution.order[first_customer_index], solution.order[idx]);
        first_customer_index = idx;
        int first_customer = solution.order[idx];
        have[first_customer] = 1;
        capacity = problem.customer_demand.at(first_customer);
        idx++;

        for(int customer : nearest[first_customer]){
            if(have[customer]) continue;
            if(capacity + problem.customer_demand.at(customer) <= problem.MAX_CAPACITY) {
                have[customer] = 1;
                capacity += problem.customer_demand.at(customer);
                solution.index_of_customer[solution.order[idx]] = solution.index_of_customer[customer];
                std::swap(solution.order[idx], solution.order[solution.index_of_customer[customer]]);
                idx++;
            } else{
                solution.tours.push_back({first_customer_index, idx - 1});
                solution.num_of_tours++;
                break;
            }
        }
    }
    solution.tours.push_back({first_customer_index, problem.NUM_OF_CUSTOMERS - 1});
    solution.num_of_tours++;
    // if (TYPE == 1){
    //     redistribute_customer();
    // }

    return solution;
}