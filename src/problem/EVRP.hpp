#pragma once

#include <string>
#include <vector>

struct node {
    int id = 0;
    double x = 0.0;
    double y = 0.0;
};

class EVRP {
public:
    std::string problem_instance;
    std::vector<node> node_list;
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

};
