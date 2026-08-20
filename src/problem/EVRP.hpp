#pragma once

#include <string>
#include <utility>
#include <vector>

struct node {
    int id = 0;
    double x = 0.0;
    double y = 0.0;
};

class EVRP {
public:
    std::string problem_instance;
    node *node_list = nullptr;
    int problem_size = 0;
    double energy_consumption = 0.0;
    int DEPOT = 0;
    int NUM_OF_CUSTOMERS = 0;
    int ACTUAL_PROBLEM_SIZE = 0;
    int NUM_OF_STATIONS = 0;
    int BATTERY_CAPACITY = 0;
    int MAX_CAPACITY = 0;
    int MIN_VEHICLES = 0;

    std::vector<int> customer_demand;
    std::vector<bool> charging_station;

    EVRP() = default;

    ~EVRP() {
        delete[] node_list;
    }

    EVRP(const EVRP&) = delete;
    EVRP& operator=(const EVRP&) = delete;
    EVRP(EVRP&& other) noexcept {
        *this = std::move(other);
    }

    EVRP& operator=(EVRP&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        delete[] node_list;
        problem_instance = std::move(other.problem_instance);
        node_list = other.node_list;
        problem_size = other.problem_size;
        energy_consumption = other.energy_consumption;
        DEPOT = other.DEPOT;
        NUM_OF_CUSTOMERS = other.NUM_OF_CUSTOMERS;
        ACTUAL_PROBLEM_SIZE = other.ACTUAL_PROBLEM_SIZE;
        NUM_OF_STATIONS = other.NUM_OF_STATIONS;
        BATTERY_CAPACITY = other.BATTERY_CAPACITY;
        MAX_CAPACITY = other.MAX_CAPACITY;
        MIN_VEHICLES = other.MIN_VEHICLES;
        customer_demand = std::move(other.customer_demand);
        charging_station = std::move(other.charging_station);
        other.node_list = nullptr;
        return *this;
    }
};
