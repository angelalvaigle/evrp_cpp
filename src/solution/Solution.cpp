#include "Solution.hpp"

#include "../problem/EVRP.hpp"

#include <cmath>

namespace {

    double get_distance(const EVRP& problem, int from, int to) {
        const node& origin = problem.node_list.at(from);
        const node& destination = problem.node_list.at(to);
        const double x = origin.x - destination.x;
        const double y = origin.y - destination.y;
        return std::sqrt(x * x + y * y);
    }

    double get_energy_consumption(const EVRP& problem, int from, int to) {
        return get_distance(problem, from, to) * problem.energy_consumption;
    }

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
            capacity -= problem.customer_demand.at(to);
        }

        energy -= get_energy_consumption(problem, from, to);
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

bool Solution::is_valid_solution(const EVRP& problem) const {
    return check_solution(problem);
}

int Solution::get_steps() const {
    return steps;
}
