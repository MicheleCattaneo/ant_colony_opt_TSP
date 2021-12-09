#include <iostream>
// #include <fstream>
// #include <cmath>
#include <math.h>
#include <vector>
#include <bitset>
#include <stdlib.h>
#include <random>
#include <float.h>
#include <string>
#include "Bitmap.hpp"
#include "Ant.hpp"
// #include "utils.hpp"
#include "2opt.cpp"
#include "NN.cpp"

// #include "2opt.cpp"

// How mow to exploit the cumulated knowledge
#define ALPHA 1.0
// How much to explore based on distances
#define BETA 2.2
#define INITIAL_TAU 0.0018
#define EVAPORATE 0.95
#define Q 1.0
// #define Q_0 0.90

// TODO CHECK CODE
int get_next_best_city(int i, std::vector<std::vector<double> > *phero, std::vector<std::vector<double> > *dist,  Bitmap * visited) {
    double best_exploitation = 0.0;
    int n = (*dist).size();
    int best_c = -1;
    double curr_ = 0.0;
    for(int j=0;j<n;++j) {
        if ( (*visited).get(j) == 0 && i != j) {
            curr_ = pow((*phero)[i][j], ALPHA)  * pow( (1.0/(*dist)[i][j]) , BETA);
            if (curr_ > best_exploitation) {
                best_exploitation = curr_;
                best_c = j;
            }
        }
    }

    return best_c;
}

int get_next_city(int i, int to_ignore, double prob, std::vector<std::vector<double> > *phero, std::vector<std::vector<double> > *dist,  Bitmap * visited) {
    int n = (*dist).size();
    // get denominator
    double tau_tot = 0.0;
    for (int k=0;k<n;++k) {
        // TODO SKIP IGNORED CITY??
        if ( (*visited).get(k) == 0 && i != k) {
            tau_tot += pow((*phero)[i][k], ALPHA) * pow((1.0/(*dist)[i][k]), BETA);
        }
    }

    double prev = 0.0;
    double curr_prob = 0.0;
    for(int j=0;j<n;++j) {
        if ( (*visited).get(j) == 0 && i != j && j != to_ignore) {// if not visited and not the best city
            curr_prob = prev + (pow((*phero)[i][j], ALPHA) * pow((1.0/(*dist)[i][j]), BETA))/tau_tot;
            if (prob <= curr_prob) {
                return j;
            }
            prev = curr_prob;
        }
    }
    // if no best city found, it means we finished the loop without finding a city to explore
    // this is a tricky case in which the best and the last city to visit are the same, so we can not ignore it
    return to_ignore;
}

int aco_solution_improved(const char * problem, unsigned seed, int ants_number, bool print_path) {
    int best_known = 0;
    std::vector<std::vector<double> > *mat = get_matrix(problem, &best_known);
    int n_cities = (*mat).size();
    // double Q_0 = 1.0-(13.0/double(n_cities));
    double Q_0 = 0.98;
    double local_evaporate = 0.1;
    double global_evaporate = 0.1;
    std::vector<int> best_global_path(n_cities);
    
    // get initial pheromone in relation with the size of the NN
    int nn_len =  NN(mat, 0);
    
    int best_global_length = nn_len;
    double tau_0 = pow((n_cities*double(nn_len)),-1);
    std::vector<std::vector<double> > phero = std::vector<std::vector<double> >(n_cities, std::vector<double> (n_cities, tau_0));

    // initialize ants
    std::vector<Ant *> ants(ants_number);
    for(int a=0; a<ants_number; ++a) {
        ants[a] = new Ant(n_cities);
    }
    std::default_random_engine generator (seed);
    std::uniform_real_distribution<double> distr(0, 1);

    int n_iterations = 100;

    for (int i=0; i<n_iterations; ++i) { // for each iteration:
        std::cout << i << std::endl;
        // towards the end, enforce exploration
        // if (i == n_iterations - 100) {
        //     Q_0 = 0.85;
        //     global_evaporate = 0.2;
        // }
        
        // reset ants and position them randomly:
        for(int a=0; a < ants_number; a++) {
            ants[a]->visited.clear();
            ants[a]->tour_len = 0;
            int ant_city = distr(generator) * n_cities;
            ants[a]->tour[0] = ant_city;
            ants[a]->visited.set(ant_city,1);
        }

        // make each ant make a step untill they all make a tour
        for(int c=1; c < n_cities; c++) {

            for(int a=0; a < ants_number; a++) {
                int last_ant_city = ants[a]->tour[c-1];

                // select next city for ant a
                int next_city = get_next_best_city(last_ant_city, &phero, mat, &(ants[a]->visited));
                double rand_0_1_q = distr(generator);
                if (rand_0_1_q > Q_0) { // explore and ignore best city
                    double rand_0_1_c = distr(generator);
                    next_city = get_next_city(last_ant_city, next_city, rand_0_1_c, &phero, mat, &(ants[a]->visited));
                }
                ants[a]->tour[c] = next_city;
                ants[a]->tour_len += (*mat)[last_ant_city][next_city];
                ants[a]->visited.set(next_city, 1);

                // local pheromone update
                double old_phero = phero[last_ant_city][next_city];
                phero[last_ant_city][next_city] = (1.0 - local_evaporate)*old_phero + local_evaporate*tau_0;
                phero[next_city][last_ant_city] = phero[last_ant_city][next_city];
            }
        }
        // add last edge and get best tour
        for(int a=0; a<ants_number; a++) {
            int city_i = ants[a]->tour[0];
            int city_j = ants[a]->tour[n_cities-1];
            // TODO add last local pheromone update??? it is never set
            ants[a]->tour_len += (*mat)[city_i][city_j];
            if(ants[a]->tour_len < best_global_length) {
                best_global_length = ants[a]->tour_len;
                best_global_path = ants[a]->tour;
            }
        }


        // apply local search (2opt) with best ant (theoretically any ant or best k ants can work)
        // an ant that is not currently the best could be placed in a local area where the global minimum is.
        best_global_length = loop2opt(&best_global_path, mat, best_global_length);

        // global evaporate on all edges
        // for(int i=0; i<n_cities; i++) {
        //     for(int j=0; j<n_cities; j++) {
        //         double old_phero = phero[i][j];
        //         phero[i][j] = phero[j][i] = (1.0 - global_evaporate)*old_phero;
        //         //  = phero[i][j];
        //     }
        // }
        
        // global update on best tour
        for(int k=0; k<n_cities-1; k++) {
            int city_i = best_global_path.at(k);
            int city_j = best_global_path.at(k+1);
            double old_phero = phero[city_i][city_j];
            phero[city_i][city_j] = (1.0 - global_evaporate)*old_phero + global_evaporate  * 1.0/best_global_length;
            phero[city_j][city_i] = phero[city_i][city_j];
        }
        // update last edge
        int city_i = best_global_path.at(0);
        int city_j = best_global_path.at(n_cities-1);
        double old_phero = phero[city_i][city_j];
        phero[city_i][city_j] = (1.0 - global_evaporate)*old_phero + global_evaporate  * 1.0/best_global_length;
        phero[city_j][city_i] = phero[city_i][city_j];
    }

    std::cout << "Best lenght found: " << best_global_length << std::endl;
    std::cout << "GAP IS " << gap(best_global_length, best_known) <<std::endl;
    if(print_path) {
        for(int i=0;i<n_cities;i++) {
            std::cout << best_global_path[i] << ',';
        }
    }
    return best_global_length;
}

// int aco_solution(const char * problem, unsigned seed, int ants_number, bool print_path) {
//     int best_known = 0;
//     std::vector<std::vector<double> > *mat = get_matrix(problem, &best_known);
//     int n = (*mat).size();
    
//     int nn_len =  NN(mat, 0);
//     std::cout << "NN length = " << nn_len << std::endl;

//     double Q_0 = 1.0-(13.0/double(n)); // TODO fix it


//     std::vector<std::vector<double> > phero = std::vector<std::vector<double> >(n, std::vector<double> (n, INITIAL_TAU));

//     std::vector<Ant *> ants(ants_number);
//     for(int a=0; a<ants_number; ++a) {
//         ants[a] = new Ant(n);
//     }

//     std::default_random_engine generator (seed);
//     std::uniform_real_distribution<double> distr(0, 1);

//     double best_solution = DBL_MAX;
//     std::vector<int> best_tour;

//     int n_iterations = 500;

//     for (int i=0; i<n_iterations; ++i) { // for each iteration of the program
//         // std::cout << i << std::endl;
//         // TODO: KEEP AN EXTRA ANT THAT STOREST THE BEST TOUR
//         // MAYBE IN THE NEXT ITERATION THE SOLUTION GETS WORSE AND YOU LOSE IT!!
//         for(int a=0; a<ants_number; ++a) { // for each ant, make a tour
//             // std::vector<int> tour(n);
//             // double tour_len = 0.0;
//             ants[a]->visited.clear();
//             ants[a]->tour_len = 0;
//             // int curr_c = rand() % n; // random first city for ant 'a'

//             int curr_c = distr(generator) * n;

//             // int curr_c = int(distr(eng)*n);
//             ants[a]->tour[0] = curr_c;
//             ants[a]->visited.set(curr_c,1);// right?

//             // chose next city
//             for(int c=1; c<n; c++) {// c++, hehe

//                 // Possible bottleneck here, avoid complete computation?
//                 // setCDF(&CDF, curr_c, &phero, mat, &(ants[a]->visited));
//                 int next_city = 0;
//                 double rand_0_1 = distr(generator);

//                 //if Q < Q0 -> Exploitation
//                 // TODO: Always compute best city, then dont consider it in second case.
//                 if (rand_0_1 < Q_0) {
//                     next_city = get_next_best_city(curr_c, &phero, mat, &(ants[a]->visited));
//                 } else { // else Exploration
//                     rand_0_1 = distr(generator);
//                     next_city = get_next_city(curr_c, -1,rand_0_1, &phero, mat, &(ants[a]->visited));
//                 }
//                 // loop the CDF and find the first index where the randomly generated number is greater-equal. That will be the next city.

//                 // next_city = binarySearchCDF(CDF, rand_0_1);
//                 ants[a]->tour[c] = next_city;
//                 ants[a]->tour_len += (*mat)[curr_c][next_city];
//                 curr_c = next_city;
//                 ants[a]->visited.set(curr_c, 1);

//             }
//             // add last connection to the tour len
//             ants[a]->tour_len += (*mat)[ants[a]->tour[0]][ants[a]->tour[n-1]];
//             // std::cout << ants[a]->tour[0] << ' ' << ants[a]->tour[n-1] << std::end;

//         }
//         // decay
//         for(int i=0;i<n;++i) {
//             for(int j=0; j<=i; ++j) {
//                 double curr_val = phero[i][j]*EVAPORATE;
//                 phero[i][j] = curr_val;
//                 phero[j][i] = curr_val;
//             }
//         }
//         // add pheromone on used paths
//         for (int a=0; a<ants_number; ++a) {
//             double curr_tour_len = ants[a]->tour_len;
//             if(curr_tour_len < best_solution) {
//                 best_solution = curr_tour_len;
//                 // save best solution
//                 best_tour = ants[a]->tour;
//                 // std::copy(ants[a]->tour.begin(), ants[a]->tour.end(), back_inserter(best_tour));
//             }
//             for(int indx = 1; indx < n; indx++) {
//                 int i = ants[a]->tour[indx-1];
//                 int j = ants[a]->tour[indx];
//                 double curr_val = phero[i][j];
//                 double new_val = curr_val + (Q/curr_tour_len);
//                 phero[i][j] = new_val;
//                 phero[j][i] = new_val;
//             }
//         }
//     }

//     // ADD ONE iteration of 2opt (ERROR)
//     // best_solution = loop2opt(&best_tour, mat, best_solution);

//     std::cout << "Best solution: " << best_solution << std::endl;
//     std::cout << "GAP = " << gap(best_solution, best_known) << std::endl;
//     if (print_path) {
//         for(auto itr = best_tour.begin(); itr != best_tour.end(); itr++) {
//             std::cout << *itr << ',';
//         }
//         std::cout << std::endl;
//     }
//     return best_solution;

// }


int main(int argc, const char ** argv) {
    unsigned seed = 123456;
    if(argc == 3) {
        seed = atoi(argv[2]);
        std::cout << "seed" << seed << std::endl;
    }
    // for (unsigned x = 0; x < 1; x++) {
    //      int solution = aco_solution_improved(argv[1], x, 10, true);
    // }
    int solution = aco_solution_improved(argv[1], seed, 10, true);
    // assumed param is given, no checks
    // std::vector<std::vector<double> > *mat = get_matrix(argv[1]);
    // int n = (*mat).size();
    // std::vector<std::vector<double> > phero = std::vector<std::vector<double> >(n, std::vector<double> (n, INITIAL_TAU));

    // initialize ants
    // int n_ants = n < 200 ? n: 50; // TODO fix this values
    // int n_ants = 10;
    // std::vector<Ant *> ants(n_ants);
    // for(int a=0; a<n_ants; ++a) {
    //     ants[a] = new Ant(n);
    // }

    // int n_iterations = 200;

    // Bitmap visited(n);
    // std::vector<double> CDF(n);
    // srand(time(0));

    // initialize uniform distribution between 0 and 1
    // std::random_device rd;  // Will be used to obtain a seed for the random number engine
    // std::mt19937 eng(rd());
    // std::random_device rd;
    
    // std::default_random_engine generator (seed);
    // std::uniform_real_distribution<double> distr(0, 1);

    // double best_solution = DBL_MAX;
    // std::vector<int> best_tour;

    // for (int i=0; i<n_iterations; ++i) { // for each iteration of the program
    //     // std::cout << i << std::endl;
    //     for(int a=0; a<n_ants; ++a) { // for each ant, make a tour
    //         // std::vector<int> tour(n);
    //         // double tour_len = 0.0;
    //         ants[a]->visited.clear();
    //         ants[a]->tour_len = 0;
    //         // int curr_c = rand() % n; // random first city for ant 'a'

    //         int curr_c = distr(generator) * n;

    //         // int curr_c = int(distr(eng)*n);
    //         ants[a]->tour[0] = curr_c;
    //         ants[a]->visited.set(curr_c,1);// right?

    //         // chose next city
    //         for(int c=1; c<n; c++) {// c++, hehe

    //             // Possible bottleneck here, avoid complete computation?
    //             // setCDF(&CDF, curr_c, &phero, mat, &(ants[a]->visited));
    //             int next_city = 0;
    //             double rand_0_1 = distr(generator);

    //             //if Q < Q0 -> Exploitation
    //             if (rand_0_1 < Q_0) {
    //                 next_city = get_next_best_city(curr_c, &phero, mat, &(ants[a]->visited));
    //             } else { // else Exploration
    //                 rand_0_1 = distr(generator);
    //                 next_city = get_next_city(curr_c, rand_0_1, &phero, mat, &(ants[a]->visited));
    //             }
    //             // loop the CDF and find the first index where the randomly generated number is greater-equal. That will be the next city.

    //             // next_city = binarySearchCDF(CDF, rand_0_1);
    //             ants[a]->tour[c] = next_city;
    //             ants[a]->tour_len += (*mat)[curr_c][next_city];
    //             curr_c = next_city;
    //             ants[a]->visited.set(curr_c, 1);

    //         }
    //         // add last connection to the tour len
    //         ants[a]->tour_len += (*mat)[ants[a]->tour[0]][ants[a]->tour[n-1]];
    //         // std::cout << ants[a]->tour[0] << ' ' << ants[a]->tour[n-1] << std::end;


    //     }
    //     // decay
    //     for(int i=0;i<n;++i) {
    //         for(int j=0; j<=i; ++j) {
    //             double curr_val = phero[i][j]*EVAPORATE;
    //             phero[i][j] = curr_val;
    //             phero[j][i] = curr_val;
    //         }
    //     }
    //     // add pheromone on used paths
    //     for (int a=0; a<n_ants; ++a) {
    //         double curr_tour_len = ants[a]->tour_len;
    //         if(curr_tour_len < best_solution) {
    //             best_solution = curr_tour_len;
    //             // save best solution
    //             best_tour = ants[a]->tour;
    //             // std::copy(ants[a]->tour.begin(), ants[a]->tour.end(), back_inserter(best_tour));
    //         }
    //         for(int indx = 1; indx < n; indx++) {
    //             int i = ants[a]->tour[indx-1];
    //             int j = ants[a]->tour[indx];
    //             double curr_val = phero[i][j];
    //             double new_val = curr_val + (Q/curr_tour_len);
    //             phero[i][j] = new_val;
    //             phero[j][i] = new_val;
    //         }
    //     }
    // }
    // std::cout << "Best solution: " << best_solution << std::endl;
    // for(auto itr = best_tour.begin(); itr != best_tour.end(); itr++) {
    //     std::cout << *itr << ',';
    // }
    // std::cout << std::endl;
    // TODO: cleanup dyn. allocated memory
}