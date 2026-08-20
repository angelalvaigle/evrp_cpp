// EVRPFileReader.cpp
#include "EVRPFileReader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

EVRP EVRPFileReader::read_problem(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Unable to open EVRP instance: " + path.string());
    }

    EVRP problem;
    problem.problem_instance = path.stem().string();
    std::string line;
    std::string section;

    while (std::getline(input, line)) {
        std::istringstream values(line);
        std::string key;
        values >> key;
        if (key.empty() || key[0] == '#') {
            continue;
        }
        if (!key.empty() && key.back() == ':') {
            key.pop_back();
        }
        if (key == "NODE_COORD_SECTION" || key == "DEMAND_SECTION" ||
            key == "STATIONS_COORD_SECTION" || key == "DEPOT_SECTION") {
            section = key;
            continue;
        }
        if (key == "EOF") {
            break;
        }
        if (section == "NODE_COORD_SECTION") {
            node current;
            if (values >> current.id >> current.x >> current.y) {
                --current.id;
                if (!problem.node_list) {
                    problem.ACTUAL_PROBLEM_SIZE = problem.problem_size + problem.NUM_OF_STATIONS;
                    problem.node_list = new node[problem.ACTUAL_PROBLEM_SIZE];
                }
                problem.node_list[current.id] = current;
            }
            continue;
        }
        if (section == "DEMAND_SECTION") {
            int id;
            int demand;
            if (values >> id >> demand) {
                if (problem.customer_demand.empty()) {
                    problem.customer_demand.resize(problem.ACTUAL_PROBLEM_SIZE, 0);
                }
                problem.customer_demand[id - 1] = demand;
            }
            continue;
        }
        if (section == "DEPOT_SECTION") {
            int depot;
            if (values >> depot && depot != -1) {
                problem.DEPOT = depot - 1;
            }
            continue;
        }

        std::string value;
        values >> value;
        if (key == "DIMENSION") {
            problem.problem_size = std::stoi(value);
            problem.NUM_OF_CUSTOMERS = problem.problem_size - 1;
        } else if (key == "STATIONS") {
            problem.NUM_OF_STATIONS = std::stoi(value);
        } else if (key == "CAPACITY") {
            problem.MAX_CAPACITY = std::stoi(value);
        } else if (key == "VEHICLES") {
            problem.MIN_VEHICLES = std::stoi(value);
        } else if (key == "ENERGY_CAPACITY") {
            problem.BATTERY_CAPACITY = std::stoi(value);
        } else if (key == "ENERGY_CONSUMPTION") {
            problem.energy_consumption = std::stod(value);
        }
    }

    problem.ACTUAL_PROBLEM_SIZE = problem.problem_size + problem.NUM_OF_STATIONS;
    if (!problem.node_list) {
        problem.node_list = new node[problem.ACTUAL_PROBLEM_SIZE];
    }
    problem.customer_demand.resize(problem.ACTUAL_PROBLEM_SIZE, 0);
    problem.charging_station.assign(problem.ACTUAL_PROBLEM_SIZE, false);
    if (problem.DEPOT >= 0 && problem.DEPOT < problem.ACTUAL_PROBLEM_SIZE) {
        problem.charging_station[problem.DEPOT] = true;
    }
    for (int station = problem.problem_size; station < problem.ACTUAL_PROBLEM_SIZE; ++station) {
        problem.charging_station[station] = true;
    }
    return problem;
}
