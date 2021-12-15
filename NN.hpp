#ifndef NN_HPP
#define NN_HPP
// #include "Bitmap.hpp"
#include <limits>
#include <vector>
#include "Bitmap.hpp"

/**
 * @brief Executes Nearest Neighbour algorithm starting at the given city
 * 
 * @param mat the distance matrix 
 * @param start_city the initial city 
 * @return int the length found.
 */
int NN(std::vector<std::vector<double>> *mat, int start_city) {
    int curr_city = start_city;
    // random starting city:
    int n = (*mat).size();
    Bitmap visited(n);
    visited.set(start_city, 1);
    int total_dist = 0;
    for (int i=1; i<n;++i) { // for each city
        int best_dist = std::numeric_limits<int>::max();
        int best_city = -1;
        for(int j=0; j<n; ++j) { // find next city
            if (curr_city != j && !visited.get(j)) {
                int dist = (*mat)[curr_city][j];
                if(dist < best_dist) {
                    // std::cout << 's' <<std::endl;
                    best_dist = dist;
                    best_city = j;
                }
            }
        }
        // std::cout << best_dist << std::endl;
        total_dist += best_dist;
        visited.set(best_city, 1);
        curr_city = best_city;
    }
    // add last node:
    total_dist += (*mat)[start_city][curr_city];

    return total_dist;
}
#endif