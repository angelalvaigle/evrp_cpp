#include "NearestNeighborAlgorithm.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

namespace {

double distance(const EVRP& problem, int from, int to) {
    const node& origin = problem.node_list.at(from);
    const node& destination = problem.node_list.at(to);
    const double x = origin.x - destination.x;
    const double y = origin.y - destination.y;
    return std::sqrt(x * x + y * y);
}

bool is_customer(const EVRP& problem, int node_id) {
    return node_id != problem.DEPOT && !problem.charging_station.at(node_id);
}

int reachable_station(const EVRP& problem, int from, int to, double battery) {
    int best_station = -1;
    double best_distance = std::numeric_limits<double>::max();
    for (int station = 0; station < problem.ACTUAL_PROBLEM_SIZE; ++station) {
        if (!problem.charging_station.at(station) || station == from) {
            continue;
        }
        const double to_station = distance(problem, from, station);
        const double station_to_target = distance(problem, station, to);
        if (to_station <= battery && station_to_target <= problem.BATTERY_CAPACITY &&
            to_station < best_distance) {
            best_station = station;
            best_distance = to_station;
        }
    }
    return best_station;
}

void append_leg(const EVRP& problem, int target, std::vector<int>& route,
                double& battery, double& total_distance) {
    const int current = route.back();
    const double leg_distance = distance(problem, current, target);
    if (leg_distance > battery) {
        const int station = reachable_station(problem, current, target, battery);
        if (station < 0) {
            throw std::runtime_error("No se puede alcanzar el siguiente nodo con la bateria disponible");
        }
        total_distance += distance(problem, current, station);
        route.push_back(station);
        battery = problem.BATTERY_CAPACITY;
    }

    const double final_leg = distance(problem, route.back(), target);
    if (final_leg > battery) {
        throw std::runtime_error("No se puede alcanzar el siguiente nodo con la bateria disponible");
    }
    total_distance += final_leg;
    route.push_back(target);
    battery -= final_leg;
    if (problem.charging_station.at(target)) {
        battery = problem.BATTERY_CAPACITY;
    }
}

} // namespace

Solution NearestNeighborAlgorithm::solve(const EVRP& problem) const {
    if (problem.node_list.empty() || problem.DEPOT < 0 ||
        problem.DEPOT >= problem.ACTUAL_PROBLEM_SIZE) {
        throw std::invalid_argument("La instancia EVRP no tiene un deposito valido");
    }

    std::vector<bool> served(problem.ACTUAL_PROBLEM_SIZE, false);
    served[problem.DEPOT] = true;
    int remaining_customers = 0;
    for (int node_id = 0; node_id < problem.ACTUAL_PROBLEM_SIZE; ++node_id) {
        if (is_customer(problem, node_id)) {
            ++remaining_customers;
        }
    }

    Solution solution;
    while (remaining_customers > 0) {
        std::vector<int> route{problem.DEPOT};
        double battery = problem.BATTERY_CAPACITY;
        int load = 0;
        int current = problem.DEPOT;

        while (true) {
            int next = -1;
            double best_distance = std::numeric_limits<double>::max();
            for (int candidate = 0; candidate < problem.ACTUAL_PROBLEM_SIZE; ++candidate) {
                if (served[candidate] || !is_customer(problem, candidate) ||
                    load + problem.customer_demand.at(candidate) > problem.MAX_CAPACITY) {
                    continue;
                }
                const double direct_distance = distance(problem, current, candidate);
                const bool reachable = direct_distance <= battery ||
                    reachable_station(problem, current, candidate, battery) >= 0;
                if (reachable && direct_distance < best_distance) {
                    next = candidate;
                    best_distance = direct_distance;
                }
            }

            if (next < 0) {
                break;
            }
            append_leg(problem, next, route, battery, solution.total_distance);
            current = next;
            load += problem.customer_demand.at(next);
            served[next] = true;
            --remaining_customers;
        }

        if (route.size() == 1) {
            throw std::runtime_error("Existe un cliente que no cabe en ningun vehiculo");
        }
        append_leg(problem, problem.DEPOT, route, battery, solution.total_distance);
        solution.routes.push_back(std::move(route));
    }

    return solution;
}