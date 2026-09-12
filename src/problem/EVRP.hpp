#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct node {
    std::string id;
    double x = 0.0;
    double y = 0.0;
};

class EVRP {
public:
    double get_distance(int from, int to) const;
    double get_energy_consumption(int from, int to) const;
    void compute_distances();
    std::vector<std::vector<int>> compute_nearest_points() const;
    const std::vector<std::vector<int>>& get_nearest_points() const;
    int get_customer_demand(int customer) const;

    std::string problem_instance;
    std::vector<node> node_list;
    std::unordered_map<std::string, int> node_index;
    int problem_size = 0;
    double energy_consumption = 0.0;
    int DEPOT = 0;
    int NUM_OF_CUSTOMERS = 0;
    int ACTUAL_PROBLEM_SIZE = 0;
    int NUM_OF_STATIONS = 0;
    int BATTERY_CAPACITY = 0;
    int MAX_CAPACITY = 0;
    int MIN_VEHICLES = 0;

    std::vector<int> cust_demand;
    std::vector<bool> charging_station;

    

private:
    std::vector<std::vector<double>> distances_;
    mutable std::vector<std::vector<int>> nearest_points_;
    mutable bool nearest_points_computed_ = false;
};
