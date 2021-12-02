#include <vector>
#include <iostream>
#include <algorithm>
#include "utils.hpp"

// GAIN = (a,d)+(b,c)-(c,d)-(a,b)
// Negative value if better path is found by swapping i and j
int gain(int i, int j, std::vector<std::vector<double> > *dist, std::vector<int> *path) {

    int old_link_len = (*dist)[(*path)[i]][(*path)[i - 1]] + (*dist)[(*path)[j]][(*path)[j + 1]];
    int changed_links_len = (*dist)[(*path)[j]][(*path)[i - 1]] + (*dist)[(*path)[i]][(*path)[j + 1]];

    return -old_link_len + changed_links_len;
}

void swap2opt(std::vector<int> *path, int i, int j) {
    std::reverse(path->begin()+i, path->begin()+j);
}



int step_2OPT(std::vector<int> *path, std::vector<std::vector<double> > *dist, int curr_len) {
    int best_gain = 0;
    int n = (*path).size(); // N is size of path??
    for (int i=1; i<n-1; ++i) {
        for(int j=i+1;j<n; ++j) {
            int curr_gain = gain(i, j, dist, path);
            int new_distance = curr_len + curr_gain;
            // std::cout << curr_gain <<std::endl;
            if (new_distance < curr_len) {
                
                swap2opt(path, i ,j+1);
                curr_len = new_distance;
                // std::cout << curr_len << std::endl;
                // break;
            }
        }
    }
    return curr_len;
}

void loop2opt(std::vector<int> *path, std::vector<std::vector<double> > *dist, int curr_len) {
    int best_gain = 0;
    int loops = 100;
    int new_len;
    do {
        new_len = step_2OPT(path, dist, curr_len);
        curr_len = new_len;
        // std::cout << best_gain <<std::endl;
        loops--;
    }
    while(loops > 0);
    std::cout << "2OPT result: "<< new_len <<std::endl;
}

int main() {
    std::vector<int> v = {42,103,106,126,33,16,30,26,18,99,115,23,14,28,94,78,86,11,80,102,76,93,88,109,97,67,62,69,96,25,6,87,68,63,85,123,128,60,108,75,44,15,127,104,61,111,116,2,113,107,7,17,20,32,21,36,46,39,22,121,54,59,50,41,43,92,3,53,34,19,45,79,117,1,49,129,70,38,40,0,4,10,82,29,58,120,77,89,124,84,65,27,114,125,12,66,95,13,9,101,5,90,71,119,52,48,57,105,37,91,72,98,73,74,51,8,56,55,64,81,100,122,110,118,83,35,112,31,24,47};
    int l = 6833;
    std::vector<std::vector<double> > *mat = get_matrix("../AI_cup_2021_problems/ch130.tsp");
    loop2opt(&v, mat,l);
    for(int i=0; i<v.size(); ++i) {
        std::cout << v[i] << ',';
    }
}
