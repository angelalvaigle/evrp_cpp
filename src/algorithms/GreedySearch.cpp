#include "GreedySearch.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>

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

    int nearest_station(const EVRP& problem, int from, int to, double energy) {
        static double min_length, length;
        min_length = SolverParameters::INF;
        static int best_station;
        best_station = -1;

        for(int v = problem.NUM_OF_CUSTOMERS + 1; v != problem.ACTUAL_PROBLEM_SIZE && v != 1; v++) {
            if(!problem.charging_station.at(v)){
                v = 0;
            }
            length = get_distance(problem, v, to);
            if(get_energy_consumption(problem, from, v) <= energy) {
                if(min_length > length){
                    min_length = length;
                    best_station = v;
                }
            }
        }
        return best_station;
    }

    int nearest_station_back(const EVRP& problem, int from, int to, double energy) {

        static double min_length, length1, length2;
        min_length = SolverParameters::INF;
        static int best_station;
        best_station = -1;

        for(int v = problem.NUM_OF_CUSTOMERS + 1; v != problem.ACTUAL_PROBLEM_SIZE && v != 1; v++) {
            if(!problem.charging_station.at(v)){
                v = 0;
            }
            if(get_energy_consumption(problem, from, v) <= energy) {
                length1 = get_distance(problem, from, v);
                length2 = get_distance(problem, v, to);
                if(min_length > length1 + length2){
                    min_length = length1 + length2;
                    best_station = v;
                }
            }
        }
        return best_station;
    }
    // bool is_customer(const EVRP& problem, int node_id) {
    //     return node_id != problem.DEPOT && !problem.charging_station.at(node_id);
    // }

    std::vector<std::vector<int>> compute_nearest_points(const EVRP& problem) {
        std::vector<std::vector<int>> nearest(problem.NUM_OF_CUSTOMERS + 1);

        for(int i = 1; i <= problem.NUM_OF_CUSTOMERS; i++) {
            nearest[i].assign(problem.NUM_OF_CUSTOMERS, 0);
            for(int j = 0; j < problem.NUM_OF_CUSTOMERS; j++) {
                nearest[i][j] = j + 1;
            }

            std::sort(nearest[i].begin(), nearest[i].end(), [&, i](int j, int k) {
                return get_distance(problem, i, j) < get_distance(problem, i, k);
            });
        }
        return nearest;
    }
}

Solution GreedySearch::solve(const EVRP& problem, bool type) const {
    // if (problem.node_list.empty() || problem.DEPOT < 0 ||
    //     problem.DEPOT >= problem.ACTUAL_PROBLEM_SIZE) {
    //     throw std::invalid_argument("La instancia EVRP no tiene un deposito valido");
    // }

    Solution solution;
    solution.order.resize(problem.NUM_OF_CUSTOMERS);
    solution.index_of_customer.assign(problem.NUM_OF_CUSTOMERS + 1, 0);
    solution.tour_index.assign(problem.NUM_OF_CUSTOMERS + 1, 0);
    const std::vector<std::vector<int>> nearest = compute_nearest_points(problem);
    std::vector<int> have(problem.NUM_OF_CUSTOMERS + 1, 0);
    for(int i = 0; i < problem.NUM_OF_CUSTOMERS; i++) {
        solution.order[i] = i + 1;
        solution.index_of_customer[i + 1] = i;
    }
    for(int i = 0; i < problem.NUM_OF_CUSTOMERS; i++) {
        int idx_1, idx_2;
        idx_1 = rand() % problem.NUM_OF_CUSTOMERS;
        idx_2 = rand() % problem.NUM_OF_CUSTOMERS;
        std::swap(solution.index_of_customer[solution.order[idx_1]], solution.index_of_customer[solution.order[idx_2]]);
        std::swap(solution.order[idx_1], solution.order[idx_2]);
    }
    int first_customer_index, capacity, idx;
    idx = 0;
    while(idx < problem.NUM_OF_CUSTOMERS) {
        // rand()%N return random number from 0 to N-1
        first_customer_index = rand()%(problem.NUM_OF_CUSTOMERS - idx) + idx;
        solution.index_of_customer[solution.order[idx]] = first_customer_index;
        std::swap(solution.order[first_customer_index], solution.order[idx]);
        first_customer_index = idx;
        int first_customer = solution.order[idx];
        have[first_customer] = 1;
        capacity = problem.customer_demand.at(first_customer);
        idx++;

        for(int customer : nearest[first_customer]){
            if(have[customer]) continue;
            if(capacity + problem.customer_demand.at(customer) <= problem.MAX_CAPACITY) {
                have[customer] = 1;
                capacity += problem.customer_demand.at(customer);
                solution.index_of_customer[solution.order[idx]] = solution.index_of_customer[customer];
                std::swap(solution.order[idx], solution.order[solution.index_of_customer[customer]]);
                idx++;
            } else{
                solution.tours.push_back({first_customer_index, idx - 1});
                solution.num_of_tours++;
                break;
            }
        }
    }
    solution.tours.push_back({first_customer_index, problem.NUM_OF_CUSTOMERS - 1});
    solution.num_of_tours++;
    
    if (type){ // 1 = redistribute customers
        redistribute_customer(problem, solution);
    }

    setup(problem, solution, type);

    return solution;
}

void GreedySearch::redistribute_customer(
    const EVRP& problem,
    Solution& solution
) const {
    // auto& order = solution.order;
    // auto& index_of_customer = solution.index_of_customer;

    // Modificar solution usando problem
    set_tour_index(solution);
    const std::vector<std::vector<int>> nearest = compute_nearest_points(problem);
    int customer;
    int have[problem.NUM_OF_CUSTOMERS + 1];
    for (int i = 0; i <= problem.NUM_OF_CUSTOMERS; i++){
        have[i] = 0;
    }
    double cap1 = 0, cap2 = 0;
    for(int i = solution.tours[solution.num_of_tours - 1].left; i <= solution.tours[solution.num_of_tours - 1].right; i++){
        have[solution.order[i]] = 1;
    }

    // choose a customer in last tour
    cap1 = get_capacity_of_tour(problem, solution, solution.num_of_tours - 1);
    int l = solution.tours[solution.num_of_tours - 1].left;
    int r = solution.tours[solution.num_of_tours - 1].right;
    customer = solution.order[rand() % (r - l + 1) + l];

    for(int x: nearest[customer]){
        if(have[x]) continue;
        cap2 = get_capacity_of_tour(problem, solution, solution.tour_index[x]);

        // Better ?
        if(cap1 + problem.customer_demand.at(x) <= problem.MAX_CAPACITY
            && abs(cap1 + problem.customer_demand.at(x) - (cap2 - problem.customer_demand.at(x))) < abs(cap1 - cap2)){

            // . convert
            int t = -1;
            for(int i = 0; i < solution.num_of_tours - 1; i++){
                for(int j = solution.tours[i].left; j <= solution.tours[i].right; j++){
                    if(solution.order[j] == x){
                        std::swap(solution.order[j], solution.order[j + 1]);
                        t = 0;
                    }
                }

                if(t == 0){
                    solution.tours[i].right--;
                    solution.tours[i + 1].left--;
                }
            }
            have[x] = 1;
            cap1 += problem.customer_demand.at(x);
            assert(solution.num_of_tours > 0);
            solution.tour_index[x] = solution.num_of_tours - 1;
            int l = solution.tours[solution.num_of_tours - 1].left;
            int r = solution.tours[solution.num_of_tours - 1].right;
            customer = solution.order[rand() % (r - l + 1) + l];
        } else{
            break;
        }
    }
}

void GreedySearch::set_tour_index(Solution& solution) const {
    // for (int i = 0; i < NUM_OF_CUSTOMERS; i++){
    //     cout << order[i] << " ";
    // } cout << "\n";
    for(int j = 0, k; j < solution.num_of_tours; j++) {
        // cout << j << " " << tours[j].left << " " << tours[j].right << "**** \n";
        for(k = solution.tours[j].left; k <= solution.tours[j].right; k++) {
            solution.tour_index[solution.order[k]] = j;
            // cout << order[k] << " " << j << "\n";
        }
    }
    // for (int i = 0; i < NUM_OF_CUSTOMERS; i++){
    //     cout << tour_index[order[i]] << " ";
    // }cout << "\n";
}

double GreedySearch::get_capacity_of_tour(
    const EVRP& problem,
    const Solution& solution,
    int tour_id
) const {
    double capacity = 0;
    for(int i = solution.tours[tour_id].left; i <= solution.tours[tour_id].right; i++) 
        capacity += problem.customer_demand.at(solution.order[i]);
    
    return capacity;
}

void GreedySearch::setup(const EVRP& problem, Solution& solution, bool type) const {
    set_tour_index(solution);
    solution.fitness = SolverParameters::INF;
    local_search(problem, solution);
    complete_gen(problem, solution, type);
}

void GreedySearch::local_search(const EVRP& problem, Solution& solution) const {

    static int l, r, x, y, i, j, u0, v0, u1, v1;
    static double t1, t2;
    static bool stop;
    for(int id_tour = 0; id_tour < solution.num_of_tours; id_tour++) {
        l = solution.tours[id_tour].left;
        r = solution.tours[id_tour].right;
        while (true) {
            stop = true;
            for(i = l; i <= r; ++i) {
                for(j = r; j > i; --j) {
                    u0 = solution.order[i];
                    u1 = i == l ? problem.DEPOT : solution.order[i - 1];
                    v0 = solution.order[j];
                    v1 = j == r ? problem.DEPOT : solution.order[j + 1];

                    t1 = get_distance(problem, u1, u0)+ get_distance(problem, v0, v1);
                    t2 = get_distance(problem, u1, v0) + get_distance(problem, u0, v1);

                    if (t1 > t2) {
                        for(x = i, y = j; x <= y; ++x, --y) {
                            std::swap(solution.order[x], solution.order[y]);
                        }
                        stop = false;
                    }

                }
            }
            if (stop) break;
        }
    }
}

void GreedySearch::complete_gen(const EVRP& problem, Solution& solution, bool type) const {

    // insert depot
    int cnt = 0;
    std::vector<int> gen_temp(solution.order.size() + solution.num_of_tours + 1);
    std::vector<int> full_path(solution.order.size() * 2 + solution.num_of_tours + 1);
    gen_temp[0] = problem.DEPOT;
    for(int j = 0; j < solution.num_of_tours; j++) {
        auto p = solution.tours[j];
        for(int k = p.left; k <= p.right; k++) {
            gen_temp[++cnt] = solution.order[k];
        }
        gen_temp[++cnt] = 0;
    }
    cnt = 0;

    // complete subtour from L . R
    static int l, r;
    for(int i = 0; i < solution.num_of_tours; i++){
        auto seg = solution.tours[i];
        l = seg.left + i;
        r = seg.right + i + 2;
        //insert charging station
        if(!complete_subgen(problem, solution, full_path.data(), gen_temp.data(), l, r, cnt, type)) {
            solution.fitness = SolverParameters::INF;
            return;
        }
    }
    full_path[cnt++] = problem.DEPOT;
    solution.solution.assign(full_path.begin(), full_path.begin() + cnt);
    solution.steps = cnt;
    if(!solution.check_solution(problem)) {
        solution.fitness = fitness_evaluation(problem, solution, full_path.data(), cnt, false);
        // cout << this->fitness << "\n";
        add_penalty(problem, solution);
    } else{
        solution.fitness = fitness_evaluation(problem, solution, full_path.data(), cnt, true);
    }

}

// Complete a tour from l to r
bool GreedySearch::complete_subgen(
    const EVRP& problem, Solution& solution,
    int* full_path, int* gen_temp, int l, int r, int &cnt, bool type) const {
    std::vector<int> have(r + 1, 0);
    // std::vector<double> remaining_energy(r + 1, 0.0);
    const int remaining_energy_size = std::max(
        problem.ACTUAL_PROBLEM_SIZE,
        problem.NUM_OF_CUSTOMERS * 2 + solution.num_of_tours + 1);
    std::vector<int> remaining_energy(remaining_energy_size, 0.0);
    int first_id = cnt;
    double energy = problem.BATTERY_CAPACITY;

    for(int j = l; j <= r; j++) {
        remaining_energy[j] = have[j] = 0;
    }
    static int from, to;
    // remaining_energy[i]: remaining energy after visiting point i
    remaining_energy[l] = problem.BATTERY_CAPACITY;

    int num_finding_safe = 0;
    for(int j = l; j < r; j++) {
        from = gen_temp[j];
        to = gen_temp[j + 1];
        
        if(get_energy_consumption(problem, from, to) <= energy) {
            full_path[cnt++] = from;
            energy -= get_energy_consumption(problem, from, to);
            remaining_energy[j + 1] = energy;
            continue;
        }

        bool stop = false;
        
        while(!stop){
            num_finding_safe ++;
            if(have[j] || num_finding_safe == SolverParameters::MAX_NUM_FINDING_SAFE) {
                solution.fitness = SolverParameters::INF;
                return false;
            }
            have[j] = 1;
            int best_station = nearest_station(problem, from, to, energy);
            if(best_station == -1) {
                while(have[j] && j - 1 >= l){
                    j--; 
                    while(full_path[--cnt] != gen_temp[j]);
                    energy = remaining_energy[j];
                    from = gen_temp[j];
                    to = gen_temp[j + 1];
                }
            } else {
                energy = problem.BATTERY_CAPACITY - get_energy_consumption(problem, best_station, to);
                if(to == problem.DEPOT) energy = problem.BATTERY_CAPACITY;
                if(energy <= remaining_energy[j + 1]){
                    while(have[j] && j - 1 >= l){
                        j--; 
                        while(full_path[--cnt] != gen_temp[j])
                        energy = remaining_energy[j];
                        from = gen_temp[j];
                        to = gen_temp[j + 1];
                    }
                } else{
                    full_path[cnt++] = from;
                    full_path[cnt++] = best_station;
                    remaining_energy[j + 1] = energy;
                    stop = true;
                }
            }
        }
    }
    l = first_id;
    r = cnt;
    full_path[r] = 0;
    remaining_energy[l] = problem.BATTERY_CAPACITY;
    for(int i = l + 1; i <= r; i++){
        remaining_energy[i] = remaining_energy[i - 1] - get_energy_consumption(problem, full_path[i], full_path[i - 1]);
        if(problem.charging_station.at(full_path[i])){
            remaining_energy[i] = problem.BATTERY_CAPACITY;
        }
    }
    
    if (type == 1)
        optimize_station(problem, full_path, l, r, remaining_energy, type);

    return true;
}

/****************************************************************/
/* Returns the solution quality of the solution. Taken as input */
/* an array of node indeces and its length                      */
/****************************************************************/
double GreedySearch::fitness_evaluation(
    const EVRP& problem, Solution&, int *routes, int size, bool) const {
  int i;
  double tour_length = 0.0;

  //the format of the solution that this method evaluates is the following
  //Node id:  0 - 5 - 6 - 8 - 0 - 1 - 2 - 3 - 4 - 0 - 7 - 0
  //Route id: 1 - 1 - 1 - 1 - 2 - 2 - 2 - 2 - 2 - 3 - 3 - 3
  //this solution consists of three routes:
  //Route 1: 0 - 5 - 6 - 8 - 0
  //Route 2: 0 - 1 - 2 - 3 - 4 - 0
  //Route 3: 0 - 7 - 0
  for (i = 0; i < size-1; i++)
        tour_length += get_distance(problem, routes[i], routes[i+1]);

  return tour_length;
}

/* add penalty for false indv */
void GreedySearch::add_penalty(const EVRP&, Solution& solution) const {
    solution.fitness *= SolverParameters::PENALTY;
}

void GreedySearch::optimize_station(
    const EVRP& problem, int *full_path, int l, int r,
    const std::vector<int>& remaining_energy, bool type) const {

    static double energy;
    energy = problem.BATTERY_CAPACITY;
    static int sz;
    for(int i = r; i - 2 > l; i--){
        if(!problem.charging_station.at(full_path[i - 1])){
            energy -= get_energy_consumption(problem, full_path[i], full_path[i - 1]);
            continue;
        }

        sz = 0;
        std::vector<int> _path(r - l);
        static double battery;
        battery = energy;
        int from = full_path[i];
        for(int j = i - 2; j >= l; j--){
            if(problem.charging_station.at(full_path[j])){
                break;
            }
            battery -= get_energy_consumption(problem, from, full_path[j]);
            if(battery <= 0) break;
            _path[sz++] = full_path[j];
            from = full_path[j];
        }

        static double deltaL1, deltaL2;
        deltaL1 = get_distance(problem, full_path[i], full_path[i - 1])
                + get_distance(problem, full_path[i - 1], full_path[i - 2])
                - get_distance(problem, full_path[i], full_path[i - 2]);

        int index = 0;
        from = full_path[i];
        int best_station = full_path[i - 1];
        for(int j = 0, to; j < sz; j++){
            to = _path[j];
            int station;
            if (type){
                station = nearest_station(problem, from, to, energy);
            } else{
                nearest_station_back(problem, from, to, energy);
            }
            energy -= get_energy_consumption(problem, from, to);
            if(station != -1){
                if(j == 0){
                    if(get_distance(problem, best_station, to) > get_distance(problem, station, to)){
                        deltaL2 = get_distance(problem, from, station) + get_distance(problem, station, to) - get_distance(problem, from, to);
                        if(deltaL2 < deltaL1){
                            deltaL1 = deltaL2;
                            best_station = station;
                            index = j;
                        }
                    }
                } else{
                    deltaL2 = get_distance(problem, from, station) + get_distance(problem, station, to) - get_distance(problem, from, to);
                    // const int to_position = i - 2 - j;
                    if(deltaL2 < deltaL1 && remaining_energy[to] + get_energy_consumption(problem, station, to)<= problem.BATTERY_CAPACITY){
                        deltaL1 = deltaL2;
                        best_station = station;
                        index = j;
                    }

                }
            }
            from = _path[j];
        }
        int id = i - 1;
        for(int j = 0; j < sz; j++){
            int x = _path[j];
            if(j == index)
                full_path[id--] = best_station;
            full_path[id--] = x;
        }
        i -= index;
        energy = problem.BATTERY_CAPACITY;
    }
}