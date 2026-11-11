#include "EVRP.hpp"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>

void EVRP::compute_distances() {
    distances_.assign(
        ACTUAL_PROBLEM_SIZE,
        std::vector<double>(ACTUAL_PROBLEM_SIZE, 0.0));

    for (int from = 0; from < ACTUAL_PROBLEM_SIZE; ++from) {
        for (int to = 0; to < ACTUAL_PROBLEM_SIZE; ++to) {
            const double x = node_list[from].x - node_list[to].x;
            const double y = node_list[from].y - node_list[to].y;
            distances_[from][to] = std::sqrt(x * x + y * y);
        }
    }
}

double EVRP::get_distance(int from, int to) const {
    evaluations_ += 1.0 / static_cast<double>(ACTUAL_PROBLEM_SIZE);
    return get_stored_distance(from, to);
}

double EVRP::get_stored_distance(int from, int to) const {
    return distances_[from][to];
}

double EVRP::get_energy_consumption(int from, int to) const {
    return get_stored_distance(from, to) * energy_consumption;
}

void EVRP::reset_evaluations() const {
    evaluations_ = 0.0;
}

void EVRP::add_full_evaluation() const {
    evaluations_ += 1.0;
}

double EVRP::get_evaluations() const {
    return evaluations_;
}

std::vector<std::vector<int>> EVRP::compute_nearest_points() const {
    std::vector<std::vector<int>> nearest(NUM_OF_CUSTOMERS + 1);

    for (int customer = 1; customer <= NUM_OF_CUSTOMERS; customer++) {
        nearest[customer].resize(NUM_OF_CUSTOMERS);
        for (int index = 0; index < NUM_OF_CUSTOMERS; index++) {
            nearest[customer][index] = index + 1;
        }

        std::sort(nearest[customer].begin(), nearest[customer].end(),
            [this, customer](int left, int right) {
                return get_distance(customer, left) < get_distance(customer, right);
            });
    }

    return nearest;
}

const std::vector<std::vector<int>>& EVRP::get_nearest_points() const {
    if (!nearest_points_computed_) {
        nearest_points_ = compute_nearest_points();
        nearest_points_computed_ = true;
    }

    return nearest_points_;
}

void EVRP::reset_nearest_points() const {
    nearest_points_.clear();
    nearest_points_computed_ = false;
}

/****************************************************************/
/* Returns the demand for a specific customer                   */
/* points: from and to.                                         */
/****************************************************************/
int EVRP::get_customer_demand(int customer) const {
  if (customer == -1){
    std::cout << "Warning customer invalid - 1!\n";
  }

  return cust_demand[customer];

}

std::string EVRP::toString() const {
    std::ostringstream output;
    output << "EVRP {\n"
           << "  problem_instance: " << problem_instance << '\n'
           << "  problem_size: " << problem_size << '\n'
           << "  actual_problem_size: " << ACTUAL_PROBLEM_SIZE << '\n'
           << "  customers: " << NUM_OF_CUSTOMERS << '\n'
           << "  stations: " << NUM_OF_STATIONS << '\n'
           << "  depot: " << DEPOT << '\n'
           << "  vehicles: " << MIN_VEHICLES << '\n'
           << "  max_capacity: " << MAX_CAPACITY << '\n'
           << "  battery_capacity: " << BATTERY_CAPACITY << '\n'
           << "  energy_consumption: " << energy_consumption << '\n'
           << "  inverse_refueling_rate: " << inverse_refueling_rate << '\n'
           << "  average_velocity: " << average_velocity << '\n'
           << "  time_window_data: "
           << (ready_time.size() == node_list.size() && due_date.size() == node_list.size()
                   ? "available"
                   : "not available")
           << '\n'
           << "  service_time_data: "
           << (service_time.size() == node_list.size() ? "available" : "not available")
           << '\n'
           << "  nodes (sample):\n";

        const auto append_node = [&output, this](int index) {
        output << "    [" << index << "] " << node_list[index].id
               << " (" << node_list[index].x << ", " << node_list[index].y << ")";
        if (index < cust_demand.size()) {
            output << " demand=" << cust_demand[index];
        }
        if (index < charging_station.size() && charging_station[index]) {
            output << " charging_station=true";
        }
        if (index < ready_time.size() && index < due_date.size()) {
            output << " ready_time=" << ready_time[index]
                   << " due_date=" << due_date[index];
        }
        if (index < service_time.size()) {
            output << " service_time=" << service_time[index];
        }
        output << '\n';
    };

    append_node(DEPOT);
    for (int customer = 1; customer <= std::min(NUM_OF_CUSTOMERS, 2); ++customer) {
        append_node(customer);
    }
    for (int station = problem_size;
         station < std::min(ACTUAL_PROBLEM_SIZE, problem_size + 2);
         ++station) {
        append_node(station);
    }

    output << "}";
    return output.str();
}