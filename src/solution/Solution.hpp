#pragma once

#include <vector>

struct Segment {
    int left = 0;
    int right = -1;
};

struct Solution {
    // Genetic representation: the customers, grouped by tour ranges.
    std::vector<int> order;
    std::vector<Segment> tours;
    std::vector<int> tour_index;
    std::vector<int> index_of_customer;
    int num_of_tours = 0;

    // Decoded representation used for validation and evaluation.
    std::vector<int> solution;
    int steps = 0;
    double fitness = 0.0;

};