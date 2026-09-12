#include "Solution.hpp"

#include "../problem/EVRP.hpp"

// Recomputes the reverse mapping from customer id to its tour number.
void Solution::set_tour_index() {
    for (int tour_id = 0; tour_id < num_of_tours; tour_id++) {
        for (int index = tours[tour_id].left; index <= tours[tour_id].right; index++) {
            tour_index[order[index]] = tour_id;
        }
    }
}

// Sums the demands of all customers belonging to one tour segment.
double Solution::get_capacity_of_tour(const EVRP& problem, int tour_id) const {
    double capacity = 0;
    for (int index = tours[tour_id].left; index <= tours[tour_id].right; index++) {
        capacity += problem.get_customer_demand(order[index]);
    }
    return capacity;
}

bool Solution::check_solution(const EVRP& problem) const {
    if (solution.empty() || solution.front() != problem.DEPOT ||
        solution.back() != problem.DEPOT) {
        return false;
    }

    double energy = problem.BATTERY_CAPACITY;
    double capacity = problem.MAX_CAPACITY;
    std::vector<int> visited(problem.NUM_OF_CUSTOMERS + 1, 0);

    for (std::size_t i = 0; i + 1 < solution.size(); ++i) {
        const int from = solution[i];
        const int to = solution[i + 1];

        if (from >= 1 && from <= problem.NUM_OF_CUSTOMERS) {
            ++visited[from];
        }
        if (to >= 1 && to <= problem.NUM_OF_CUSTOMERS) {
            capacity -= problem.get_customer_demand(to);
        }

        energy -= problem.get_energy_consumption(from, to);
        if (capacity < 0.0 || energy < 0.0) {
            return false;
        }

        if (to == problem.DEPOT) {
            capacity = problem.MAX_CAPACITY;
            energy = problem.BATTERY_CAPACITY;
        } else if (problem.charging_station.at(to)) {
            energy = problem.BATTERY_CAPACITY;
        }
    }

    for (int customer = 1; customer <= problem.NUM_OF_CUSTOMERS; ++customer) {
        if (visited[customer] != 1) {
            return false;
        }
    }

    return true;
}

double Solution::get_fitness() const {
    return fitness;
}

void Solution::set_fitness(double value) {
    fitness = value;
}

double Solution::get_total_distance(const EVRP& problem) const {
    double total_distance = 0.0;
    for (std::size_t i = 0; i + 1 < solution.size(); ++i) {
        total_distance += problem.get_distance(solution[i], solution[i + 1]);
    }
    return total_distance;
}

bool Solution::is_valid_solution(const EVRP& problem) const {
    return check_solution(problem);
}

int Solution::get_steps() const {
    return steps;
}

void Solution::copy_order(const Solution& other) {
    order = other.order;
    tours = other.tours;
    tour_index = other.tour_index;
    num_of_tours = other.num_of_tours;
    fitness = other.fitness;
}