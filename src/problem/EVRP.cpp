#include "EVRP.hpp"

#include <iostream>
#include <algorithm>
#include <cmath>

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
    return distances_.at(from).at(to);
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