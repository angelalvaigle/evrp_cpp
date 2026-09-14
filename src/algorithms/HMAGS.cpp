#include "HMAGS.hpp"
#include "../solution/SolutionEvaluator.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>


const int NUM_OF_INDVS = 200;
const double PR_MUTATE = 0.1;
// const int MAX_NODE = 1500;

// int hmags_remaining_energy[MAX_NODE];
// int hmags_gen_temp[MAX_NODE];
// int hmags_full_path[MAX_NODE];
// short int hmags_path[MAX_NODE];

Solution HMAGS::solve(const EVRP& problem, bool type) const {
    problem.reset_nearest_points();
    problem.reset_evaluations();
    Solution best_sol;
    best_sol.set_fitness(SolverParameters::INF);
    Solution pop[3 * NUM_OF_INDVS];
    double rank[3 * NUM_OF_INDVS];

    init(problem, pop, best_sol, type);

    const double termination =
        25000.0 * static_cast<double>(problem.ACTUAL_PROBLEM_SIZE);
    while (problem.get_evaluations() < termination) {
        run_HMAGS(problem, pop, rank, best_sol, type); 
    }

    std::cout << "HMAGS evaluations: " << problem.get_evaluations() << '\n';
    return best_sol;
}

void HMAGS::init(const EVRP& problem, Solution pop[], Solution& best_sol, bool type) const {
    for(int i = 0; i < NUM_OF_INDVS; i++) {
        opt_generate(problem, pop[i], type);
        if (pop[i].get_fitness() < best_sol.get_fitness()){
            best_sol = pop[i];
        }
    }
}

// Builds an initial customer ordering, groups nearby customers without
// exceeding vehicle capacity, optionally redistributes customers from the
// last tour, and finally turns the groups into complete EVRP routes.
void HMAGS::opt_generate(const EVRP& problem, Solution& solution, bool type) const {
    solution.order.resize(problem.NUM_OF_CUSTOMERS);
    solution.index_of_customer.assign(problem.NUM_OF_CUSTOMERS + 1, 0);
    solution.tour_index.assign(problem.NUM_OF_CUSTOMERS + 1, 0);
    // Precompute customer proximity and initialise a permutation containing
    // every customer exactly once.
    const std::vector<std::vector<int>> nearest = problem.get_nearest_points();
    std::vector<int> have(problem.NUM_OF_CUSTOMERS + 1, 0);
    // Randomise the starting permutation so repeated greedy runs can explore
    // different customer groupings.
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
        // orig: rand()%N return random number from 0 to N-1
        // Select an unassigned customer as the seed of the next tour.
        first_customer_index = rand()%(problem.NUM_OF_CUSTOMERS - idx) + idx;
        solution.index_of_customer[solution.order[idx]] = first_customer_index;
        std::swap(solution.order[first_customer_index], solution.order[idx]);
        first_customer_index = idx;
        int first_customer = solution.order[idx];
        have[first_customer] = 1;
        capacity = problem.get_customer_demand(first_customer);
        idx++;

        // Add the nearest unassigned customers while vehicle capacity allows
        // it. The pair of indices stored in tours refers to order.
        for(int customer : nearest[first_customer]){
            if(have[customer]) continue;
            if(capacity + problem.get_customer_demand(customer) <= problem.MAX_CAPACITY) {
                have[customer] = 1;
                capacity += problem.get_customer_demand(customer);
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
    
    // orig: if TYPE==1 = redistribute customers
    // In this mode, try to balance the last tour with the preceding tours.
    if (type){
        redistribute_customer(problem, solution);
    }

    setup(problem, solution, type);
}

// Moves suitable customers into the last tour to improve the balance between
// tour loads while preserving the capacity constraint.
void HMAGS::redistribute_customer(
    const EVRP& problem,
    Solution& solution
) const {
    // auto& order = solution.order;
    // auto& index_of_customer = solution.index_of_customer;

    // orig: Modificar solution usando problem
    // Map each customer to its current tour before considering moves.
    solution.set_tour_index();
    const auto& nearest = problem.get_nearest_points();
    int customer;
    int have[problem.NUM_OF_CUSTOMERS + 1];
    for (int i = 0; i <= problem.NUM_OF_CUSTOMERS; i++){
        have[i] = 0;
    }
    double cap1 = 0, cap2 = 0;
    for(int i = solution.tours[solution.num_of_tours - 1].left; i <= solution.tours[solution.num_of_tours - 1].right; i++){
        have[solution.order[i]] = 1;
    }

    // orig: choose a customer in last tour
    // Keep the current load of the last tour and choose one of its customers
    // as the reference for the search.
    cap1 = solution.get_capacity_of_tour(problem, solution.num_of_tours - 1);
    int l = solution.tours[solution.num_of_tours - 1].left;
    int r = solution.tours[solution.num_of_tours - 1].right;
    customer = solution.order[rand() % (r - l + 1) + l];

    // Visit candidates in proximity order. A candidate is accepted only when
    // it fits in the last tour and reduces the difference between tour loads.
    for(int x: nearest[customer]){
        if(have[x]) continue;
        cap2 = solution.get_capacity_of_tour(problem, solution.tour_index[x]);

        // orig: Better ?
        if(cap1 + problem.get_customer_demand(x) <= problem.MAX_CAPACITY
            && abs(cap1 + problem.get_customer_demand(x) - (cap2 - problem.get_customer_demand(x))) < abs(cap1 - cap2)){

            // orig: . convert
            // Remove the candidate from its old tour by shifting the next
            // position into its place, then update the adjacent boundaries.
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
            cap1 += problem.get_customer_demand(x);
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

// Prepares the solution for route completion: refreshes tour indices, runs
// intra-tour 2-opt improvement, and inserts charging stations.
void HMAGS::setup(const EVRP& problem, Solution& solution, bool type) const {
    solution.set_tour_index();
    solution.set_fitness(SolverParameters::INF);
    local_search(problem, solution);
    complete_gen(problem, solution, type);
}

// Applies repeated 2-opt exchanges independently to each customer tour. An
// exchange is kept when reversing the segment shortens its two boundary legs.
void HMAGS::local_search(const EVRP& problem, Solution& solution) const {
    static int l, r, x, y, i, j, u0, v0, u1, v1;
    static double t1, t2;
    static bool stop;
    for(int id_tour = 0; id_tour < solution.num_of_tours; id_tour++) {
        l = solution.tours[id_tour].left;
        r = solution.tours[id_tour].right;
        while (true) {
            stop = true;
            // Try every pair of positions until a full pass finds no improving
            // reversal. The loop therefore stops at a local distance optimum.
            for(i = l; i <= r; ++i) {
                for(j = r; j > i; --j) {
                    u0 = solution.order[i]; //, u1 = solution.order[i - 1];;
                    u1 = i == l ? problem.DEPOT : solution.order[i - 1];
                    v0 = solution.order[j]; //, v1 = solution.order[j + 1];;
                    v1 = j == r ? problem.DEPOT : solution.order[j + 1];

                    // if(i - 1 < l)
                    //     u1 = 0;
                    // if(j + 1 > r)
                    //     v1 = 0;
                        
                    t1 = evaluator_.distance(problem, u1, u0)+ evaluator_.distance(problem, v0, v1);
                    t2 = evaluator_.distance(problem, u1, v0) + evaluator_.distance(problem, u0, v1);

                    if (t1 > t2) {
                        // Reverse the segment between i and j, corresponding to
                        // the cheaper pair of boundary edges.
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

// Converts the customer tours into a complete path containing depot visits,
// inserts charging stations when needed, and computes the final fitness.
void HMAGS::complete_gen(const EVRP& problem, Solution& solution, bool type) const {
    // orig: insert depot
    // Flatten the customer segments and place a depot after every tour. The
    // resulting temporary path is still missing charging stations.
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

    // orig: complete subtour from L . R
    // Complete each depot-delimited segment. A failure means that no safe
    // charging insertion was found within the search limit.
    static int l, r;
    for(int i = 0; i < solution.num_of_tours; i++){
        auto seg = solution.tours[i];
        l = seg.left + i;
        r = seg.right + i + 2;
        // orig: insert charging station
        //insert charging station
        if(!complete_subgen(problem, solution, full_path.data(), gen_temp.data(), l, r, cnt, type)) {
            solution.set_fitness(SolverParameters::INF);
            return;
        }
    }
    full_path[cnt++] = problem.DEPOT;
    solution.solution.assign(full_path.begin(), full_path.begin() + cnt);
    solution.steps = cnt;
    SolutionEvaluator evaluator;
    // Feasible solutions receive their distance as fitness; infeasible ones
    // receive the same distance multiplied by a penalty factor.
    if(!solution.check_solution(problem)) {
        solution.set_fitness(evaluator.fitness_evaluation(problem, solution));
        // cout << this->fitness << "\n";
        evaluator.add_penalty(solution);
    } else{
        solution.set_fitness(evaluator.fitness_evaluation(problem, solution));
    }
}

// orig: Complete a tour from l to r
// Completes one depot-delimited tour from l to r by inserting charging
// stations whenever the next leg cannot be reached with the current battery.
bool HMAGS::complete_subgen(
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

    // Initialise the backtracking state. remaining_energy stores the battery
    // level at each temporary-path position, while have prevents retry loops.
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
        
        if(problem.get_energy_consumption(from, to) <= energy) {
            full_path[cnt++] = from;
            energy -= problem.get_energy_consumption(from, to);
            remaining_energy[j + 1] = energy;
            continue;
        }

        bool stop = false;
        
        // The direct leg is not feasible. Repeatedly choose a reachable
        // station, or backtrack to an earlier leg when that choice cannot
        // leave enough energy for the remainder of the tour.
        while(!stop){
            num_finding_safe ++;
            if(have[j] || num_finding_safe == SolverParameters::MAX_NUM_FINDING_SAFE) {
                solution.set_fitness(SolverParameters::INF);
                return false;
            }
            have[j] = 1;
            int best_station = nearest_station(problem, from, to, energy);
            if(best_station == -1) {
                // No station can be reached from this point, so undo the
                // latest inserted path nodes and retry from the previous leg.
                while(have[j] && j - 1 >= l){
                    j--; 
                    while(full_path[--cnt] != gen_temp[j]);
                    energy = remaining_energy[j];
                    from = gen_temp[j];
                    to = gen_temp[j + 1];
                }
            } else {
                energy = problem.BATTERY_CAPACITY - problem.get_energy_consumption(best_station, to);
                if(to == problem.DEPOT) energy = problem.BATTERY_CAPACITY;
                if(energy <= remaining_energy[j + 1]){
                    // This station does not improve the stored energy state
                    // for the next point, so force a backtracking attempt.
                    while(have[j] && j - 1 >= l){
                        j--; 
                        while(full_path[--cnt] != gen_temp[j])
                        energy = remaining_energy[j];
                        from = gen_temp[j];
                        to = gen_temp[j + 1];
                    }
                } else{
                    // Commit the current node and selected station. The next
                    // iteration continues from the station's new charge.
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
    // Recompute energy on the actual path because inserted stations reset the
    // battery and may have changed the sequence length.
    remaining_energy[l] = problem.BATTERY_CAPACITY;
    for(int i = l + 1; i <= r; i++){
        remaining_energy[i] = remaining_energy[i - 1] - problem.get_energy_consumption(full_path[i], full_path[i - 1]);
        if(problem.charging_station.at(full_path[i])){
            remaining_energy[i] = problem.BATTERY_CAPACITY;
        }
    }
    
    if (type)
        optimize_station(problem, full_path, l, r, remaining_energy, type);

    return true;
}

// Tries to replace charging stations with shorter alternatives. It scans the
// route backwards so a replacement can be checked against the energy already
// available at the following segment.
void HMAGS::optimize_station(
    const EVRP& problem, int *full_path, int l, int r,
    const std::vector<int>& remaining_energy, bool type) const {

    static double energy;
    energy = problem.BATTERY_CAPACITY;
    static int sz;
    for(int i = r; i - 2 > l; i--){
        if(!problem.charging_station.at(full_path[i - 1])){
            energy -= problem.get_energy_consumption(full_path[i], full_path[i - 1]);
            continue;
        }

        // Collect customer positions before the current station that could be
        // reached through another station without exceeding battery capacity.
        sz = 0;
        std::vector<int> _path(r - l);
        static double battery;
        battery = energy;
        int from = full_path[i];
        for(int j = i - 2; j >= l; j--){
            if(problem.charging_station.at(full_path[j])){
                break;
            }
            battery -= problem.get_energy_consumption(from, full_path[j]);
            if(battery <= 0) break;
            _path[sz++] = full_path[j];
            from = full_path[j];
        }

        static double deltaL1, deltaL2;
        // Baseline cost: remove the current station from its two neighbouring
        // edges. Candidate stations are accepted only when they improve it.
        deltaL1 = problem.get_distance(full_path[i], full_path[i - 1])
            + problem.get_distance(full_path[i - 1], full_path[i - 2])
            - problem.get_distance(full_path[i], full_path[i - 2]);

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
            energy -= problem.get_energy_consumption(from, to);
            if(station != -1){
                if(j == 0){
                    if(problem.get_distance(best_station, to) > problem.get_distance(station, to)){
                        deltaL2 = problem.get_distance(from, station) + problem.get_distance(station, to) - problem.get_distance(from, to);
                        if(deltaL2 < deltaL1){
                            deltaL1 = deltaL2;
                            best_station = station;
                            index = j;
                        }
                    }
                } else{
                    deltaL2 = problem.get_distance(from, station) + problem.get_distance(station, to) - problem.get_distance(from, to);
                    // const int to_position = i - 2 - j;
                    if(deltaL2 < deltaL1 && remaining_energy[to] + problem.get_energy_consumption(station, to)<= problem.BATTERY_CAPACITY){
                        deltaL1 = deltaL2;
                        best_station = station;
                        index = j;
                    }

                }
            }
            from = _path[j];
        }
        // Rewrite the affected route segment with the best station found and
        // skip over the customers covered by that replacement.
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

// Finds the charging station closest to the next destination among the
// stations reachable from the current node with the available energy.
int HMAGS::nearest_station(const EVRP& problem, int from, int to, double energy) const {
    static double min_length, length;
    min_length = SolverParameters::INF;
    static int best_station;
    best_station = -1;

    for(int v = problem.NUM_OF_CUSTOMERS + 1; v != problem.ACTUAL_PROBLEM_SIZE && v != 1; v++) {
        if(!problem.charging_station.at(v)){
            v = 0;
        }
        length = evaluator_.distance(problem, v, to);
        if(problem.get_energy_consumption(from, v) <= energy) {
            if(min_length > length){
                min_length = length;
                best_station = v;
            }
        }
    }
    return best_station;
}

// Finds the station that minimises the two-leg distance from the current
// node through the station to the destination.
int HMAGS::nearest_station_back(const EVRP& problem, int from, int to, double energy) const {

    static double min_length, length1, length2;
    min_length = SolverParameters::INF;
    static int best_station;
    best_station = -1;

    for(int v = problem.NUM_OF_CUSTOMERS + 1; v != problem.ACTUAL_PROBLEM_SIZE && v != 1; v++) {
        if(!problem.charging_station.at(v)){
            v = 0;
        }
        if(problem.get_energy_consumption(from, v) <= energy) {
            length1 = evaluator_.distance(problem, from, v);
            length2 = evaluator_.distance(problem, v, to);
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

/*implement your heuristic in this function*/
void HMAGS::run_HMAGS(const EVRP& problem, Solution pop[], double rank[], Solution& best_sol, bool type) const {
    Evolution(problem, pop, rank, best_sol, type);
}

void HMAGS::Evolution(const EVRP& problem, Solution pop[], double rank[], Solution& best_sol, bool type)  const {
    Repopulation(problem, pop, rank, best_sol, type);
    Selection(problem, pop, rank);
}

void HMAGS::Repopulation(const EVRP& problem, Solution pop[], double rank[], Solution& best_sol, bool type) const {
    compute_rank(pop, rank, NUM_OF_INDVS);
    for(int i = 0; i < 2 * NUM_OF_INDVS; i += 2) {
        // choose the two parents
        double p1 = (double) rand() / (double) RAND_MAX;
        double p2 = (double) rand() / (double) RAND_MAX;
        int idx_1 = choose_by_rank(rank, p1);
        int idx_2 = choose_by_rank(rank, p2);
        while(p1 == p2)
            p2 = (double) rand() / (double) RAND_MAX;
        distribute_crossover(problem, pop[idx_1], pop[idx_2], pop, NUM_OF_INDVS + i, type);
    }
    int cnt = 0;
    for (int i = NUM_OF_INDVS; i < 3 * NUM_OF_INDVS; i++){
        if( pop[i].get_steps() == 0){
            cnt++;
        }
        if (pop[i].get_fitness() < best_sol.get_fitness()){
            best_sol = pop[i];
        }
    }
}

void HMAGS::compute_rank(Solution pop[], double rank[], int n) const {
    double sum = 0;
    double fit_min = SolverParameters::INF;
    double fit_max = 0;
    for(int i = 0; i < n; i++){
        fit_min = std::min(fit_min, pop[i].get_fitness());
        fit_max = std::max(fit_max, pop[i].get_fitness());
    }
    for(int i = 0; i < n; i++){
        double temp_fit = std::pow((fit_max - pop[i].get_fitness()) / (fit_max - fit_min + 1e-6), 2);
        sum += temp_fit;
        rank[i] = temp_fit;
    }
    for(int i = 0; i < n; i++){
        rank[i] /= sum;
        if(i > 0)
            rank[i] += rank[i - 1];
    }
}

int HMAGS::choose_by_rank(double rank[], double prob) const{
    return (int) (std::upper_bound(rank, rank + NUM_OF_INDVS, prob) - rank);
}

void HMAGS::distribute_crossover(const EVRP& problem, Solution parent_1, Solution parent_2, Solution pop[], int idx, bool type) const {
    int num = rand()%(problem.NUM_OF_CUSTOMERS) + 1;// so ngau nhien tu 1 den size of customers
    int id1 = parent_1.tour_index[num];// customer 'num' of id1 tour in parent_1
    int id2 = parent_2.tour_index[num];// customer 'num' of id2 tour in parent_2
    int have[problem.NUM_OF_CUSTOMERS + 1];
    int alens[problem.NUM_OF_CUSTOMERS + 1];

    Solution child1;
    Solution child2;
    // child1 = parent_1;   // child1.copy_order(parent_1);
    // child2 = parent_2;   // child2.copy_order(parent_2);

    child1.copy_order(parent_1);
    child2.copy_order(parent_2);

    for(int i = 0; i <= problem.NUM_OF_CUSTOMERS; i++){
        have[i] = 0;// have : exists in the alens or not
        alens[i] = 0;
    }

    int index = 0;

    // merge tour_id1, tour_id2 into alens
    for(int i = parent_1.tours[id1].right; i >= parent_1.tours[id1].left; i--) {
        alens[index++] = parent_1.order[i];
        have[parent_1.order[i]] = 1;
    }

    for(int i = parent_2.tours[id2].right; i >= parent_2.tours[id2].left; i--) {
        if(!have[parent_2.order[i]]) {
            alens[index++] = parent_2.order[i];
            have[parent_2.order[i]] = 1;
        }
    }
    int index2 = 0; // size of alens

    // Distribute alens to both of child1 and child2

    for(int i = 0; i < problem.NUM_OF_CUSTOMERS; i++){
        if(have[child1.order[i]]){
            child1.order[i] = alens[--index];
        }
        if(have[child2.order[i]]){
            child2.order[i] = alens[index2++];
        }
    }
    mutation(problem, child1);
    mutation(problem, child2);
    pop[idx].copy_order(child1);
    pop[idx + 1].copy_order(child2);
    setup(problem, pop[idx], type);
    setup(problem, pop[idx + 1], type);
}

void HMAGS::Selection(const EVRP& problem, Solution pop[], double rank[]) const {

    std::sort(pop, pop + 3 * NUM_OF_INDVS, [](Solution x, Solution y) {
        return x.get_fitness() < y.get_fitness();
    });

    compute_rank(pop, rank, 2 * NUM_OF_INDVS);

    for (int i = 0; i < NUM_OF_INDVS; i++) {
        double prob = (double) rand() / (double) RAND_MAX;
        int idx = choose_by_rank(rank, prob);
        pop[2 * NUM_OF_INDVS + i].copy_order(pop[idx]);
    }
    
    for (int i = 0; i < NUM_OF_INDVS; i++) {
        pop[i].copy_order(pop[NUM_OF_INDVS * 2 + i]);
    }
    
}

void HMAGS::mutation(const EVRP& problem, Solution& solution) const {
    const std::vector<std::vector<int>> nearest = problem.get_nearest_points();
    double mutate_prob_1 = (double) rand() / (double) RAND_MAX;
    double mutate_prob_2 = (double) rand() / (double) RAND_MAX;
    solution.set_tour_index();
    if(mutate_prob_1 < PR_MUTATE){
        int customer = rand() % (problem.NUM_OF_CUSTOMERS) + 1;
        static int near_customer;
        near_customer = -1;

        for(int x: nearest[customer]){
            if(solution.tour_index[x] != solution.tour_index[customer]){
                near_customer = x;
                break;
            }
        }

        if(near_customer != -1){

            for(int i = solution.tours[solution.tour_index[customer]].left; i <= solution.tours[solution.tour_index[customer]].right; i++){
                if(solution.order[i] == customer){
                    solution.order[i] = near_customer;
                    break;
                }
            }
            
            for(int i = solution.tours[solution.tour_index[near_customer]].left; i <= solution.tours[solution.tour_index[near_customer]].right; i++){
                if(solution.order[i] == near_customer){
                    solution.order[i] = customer;
                    break;
                }
            }

            std::swap(solution.tour_index[customer], solution.tour_index[near_customer]);
        }
        return;
    }

    if(mutate_prob_2 < PR_MUTATE){
        // choose randomly a index of order
        int customer_index = rand() % (problem.NUM_OF_CUSTOMERS);
        int customer = solution.order[customer_index];
        double cost = solution.get_capacity_of_tour(
            problem, solution.tour_index[customer]);

        int near_customer = -1;
        for(int x: nearest[customer]){
            if(solution.tour_index[x] != solution.tour_index[customer] && cost + problem.get_customer_demand(x) <= problem.MAX_CAPACITY
                    && solution.tours[solution.tour_index[x]].right - solution.tours[solution.tour_index[x]].left > 1){

                near_customer = x;
                break;
            }
        }
        if(near_customer != -1){
            int near_customer_tour_index = solution.tour_index[near_customer], customer_tour_index = solution.tour_index[customer];

            int near_customer_index = -1;
            for(int i = solution.tours[near_customer_tour_index].left; i <= solution.tours[near_customer_tour_index].right; i++){
                if(solution.order[i] == near_customer){
                    near_customer_index = i;
                    break;
                }
            }

            // pick from near_customer_tour_index . customer_tour_index
            if(customer_tour_index < near_customer_tour_index){
                std::swap(solution.order[near_customer_index], solution.order[solution.tours[near_customer_tour_index].left]);
                solution.tours[near_customer_tour_index].left++;
                for(int i = near_customer_tour_index - 1; i > customer_tour_index; i--){
                    for(int j = solution.tours[i].right; j >= solution.tours[i].left; j--){
                        solution.order[j + 1] = solution.order[j];
                    }
                    solution.tours[i].left++;
                    solution.tours[i].right++;
                }
                solution.tours[customer_tour_index].right++;
                solution.order[solution.tours[customer_tour_index].right] = near_customer;
                solution.tour_index[near_customer] = customer_tour_index;
            } else{
                std::swap(solution.order[near_customer_index], solution.order[solution.tours[near_customer_tour_index].right]);
                solution.tours[near_customer_tour_index].right--;
                for(int i = near_customer_tour_index + 1; i <= customer_tour_index; i++){
                    for(int j = solution.tours[i].left; j <= solution.tours[i].right; j++){
                        solution.order[j - 1] = solution.order[j];
                    }
                    solution.tours[i].left--;
                    solution.tours[i].right--;
                }
                solution.tours[customer_tour_index].right++;
                solution.order[solution.tours[customer_tour_index].right] = near_customer;
                solution.tour_index[near_customer] = customer_tour_index;
            }
        }
        return;
    }
    // if(mutate_prob_3 < PR_MUTATE){
    //     int customer_index = rand() % (NUM_OF_CUSTOMERS);
    //     int near_customer_index = rand() % (NUM_OF_CUSTOMERS);
    //     int customer = solution.order[customer_index];
    //     int near_customer = solution.order[near_customer_index];
    //     swap(solution.order[near_customer_index], solution.order[customer_index]);
    //     swap(solution.tour_index[near_customer], solution.tour_index[customer]);
    // }
}