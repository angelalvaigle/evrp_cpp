#include "SchneiderFileReader.hpp"

#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct SchneiderLocation {
    node location;
    char type = '\0';
    double demand = 0.0;
    double ready_time = 0.0;
    double due_date = 0.0;
    double service_time = 0.0;
};

double read_parameter(const std::string& line, const std::string& name) {
    const std::size_t start = line.find('/', line.find(name));
    const std::size_t end = line.find('/', start + 1);
    if (start == std::string::npos || end == std::string::npos) {
        throw std::runtime_error("Invalid Schneider parameter: " + name);
    }
    return std::stod(line.substr(start + 1, end - start - 1));
}

}

EVRP SchneiderFileReader::read_problem(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Unable to open Schneider instance: " + path.string());
    }

    EVRP problem;
    problem.problem_instance = path.stem().string();
    std::vector<SchneiderLocation> locations;
    std::string line;
    bool header_skipped = false;

    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }

        std::istringstream values(line);
        std::string id;
        char type;
        if (!header_skipped) {
            values >> id >> type;
            if (id == "StringID" && type == 'T') {
                header_skipped = true;
                continue;
            }
            throw std::runtime_error("Invalid Schneider header in: " + path.string());
        }

        if (line.rfind("Q Vehicle", 0) == 0) {
            problem.BATTERY_CAPACITY = read_parameter(line, "Q Vehicle");
            continue;
        }
        if (line.rfind("C Vehicle", 0) == 0) {
            problem.MAX_CAPACITY = read_parameter(line, "C Vehicle");
            continue;
        }
        if (line.rfind("r fuel", 0) == 0) {
            problem.energy_consumption = read_parameter(line, "r fuel");
            continue;
        }
        if (line.rfind("g inverse", 0) == 0) {
            problem.inverse_refueling_rate = read_parameter(line, "g inverse");
            continue;
        }
        if (line.rfind("v average", 0) == 0) {
            problem.average_velocity = read_parameter(line, "v average");
            continue;
        }

        std::string location_row = line;
        std::size_t field_count = 0;
        do {
            std::istringstream row_values(location_row);
            std::string field;
            field_count = 0;
            while (row_values >> field) {
                ++field_count;
            }
            if (field_count >= 8) {
                break;
            }
            if (!std::getline(input, line)) {
                throw std::runtime_error("Invalid location row in Schneider instance: " + location_row);
            }
            location_row += " " + line;
        } while (true);

        std::istringstream location_values(location_row);
        SchneiderLocation current;
        if (!(location_values >> current.location.id >> current.type >> current.location.x
              >> current.location.y >> current.demand >> current.ready_time
              >> current.due_date >> current.service_time)) {
            throw std::runtime_error("Invalid location row in Schneider instance: " + location_row);
        }
        if (current.type != 'd' && current.type != 'f' && current.type != 'c') {
            throw std::runtime_error("Unknown Schneider location type: " + std::string(1, current.type));
        }
        locations.push_back(current);
    }

    if (!header_skipped || locations.empty()) {
        throw std::runtime_error("Schneider instance has no locations: " + path.string());
    }

    const auto append_locations = [&problem](const std::vector<SchneiderLocation>& source, char type) {
        for (const SchneiderLocation& current : source) {
            if (current.type != type) {
                continue;
            }
            const int index = static_cast<int>(problem.node_list.size());
            problem.node_index.emplace(current.location.id, index);
            problem.node_list.push_back(current.location);
            problem.cust_demand.push_back(static_cast<int>(std::lround(current.demand)));
            problem.ready_time.push_back(current.ready_time);
            problem.due_date.push_back(current.due_date);
            problem.service_time.push_back(current.service_time);
        }
    };

    append_locations(locations, 'd');
    if (problem.node_list.size() != 1) {
        throw std::runtime_error("Schneider instance must contain exactly one depot");
    }
    append_locations(locations, 'c');
    const int customer_count = static_cast<int>(problem.node_list.size()) - 1;
    append_locations(locations, 'f');

    problem.DEPOT = 0;
    problem.problem_size = customer_count + 1;
    problem.NUM_OF_CUSTOMERS = customer_count;
    problem.NUM_OF_STATIONS = static_cast<int>(problem.node_list.size()) - problem.problem_size;
    problem.ACTUAL_PROBLEM_SIZE = static_cast<int>(problem.node_list.size());
    problem.MIN_VEHICLES = 1;
    problem.charging_station.assign(problem.ACTUAL_PROBLEM_SIZE, false);
    problem.charging_station[problem.DEPOT] = true;
    for (int station = problem.problem_size; station < problem.ACTUAL_PROBLEM_SIZE; ++station) {
        problem.charging_station[station] = true;
    }
    problem.compute_distances();
    return problem;
}