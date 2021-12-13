#ifndef TWOOPT
#define TWOOPT

#include <vector>
#include <iostream>
#include <algorithm>


// GAIN = (a,d)+(b,c)-(c,d)-(a,b)
// Negative value if better path is found by swapping i and j
int gain(int i, int j, std::vector<std::vector<double> > *dist, std::vector<int> *path) {
    int path_iminus1 = (*path)[i - 1];
    int path_jplus1 = (*path)[j+1];
    int path_i = (*path)[i];
    int path_j = (*path)[j];
    int old_link_len = (*dist)[path_i][path_iminus1] + (*dist)[path_j][path_jplus1];
    int changed_links_len = (*dist)[path_j][path_iminus1] + (*dist)[path_i][path_jplus1];
    // int old_link_len = (*dist)[(*path)[i]][(*path)[i - 1]] + (*dist)[(*path)[j]][(*path)[j+1]];
    // int changed_links_len = (*dist)[(*path)[j]][(*path)[i - 1]] + (*dist)[(*path)[i]][(*path)[j+1]];

    return -old_link_len + changed_links_len;
}

void swap2opt(std::vector<int> *path, int i, int j) {
    std::reverse(path->begin()+i, path->begin()+j+1);
}

int step_2OPT(std::vector<int> *path, std::vector<std::vector<double> > *dist, int curr_len) {
    int best_gain = 0;
    int n = (*path).size()-1; // N is size of path??
    start:
    for (int i=1; i<n-1; ++i) {
        for(int j=i+1;j<n; ++j) {
            int curr_gain = gain(i, j, dist, path);
            int new_distance = curr_len + curr_gain;
            // std::cout << curr_gain <<std::endl;
            if (new_distance < curr_len) {
                
                swap2opt(path, i ,j);
                curr_len = new_distance;
                // std::cout << 'a';
                // goto start; // To add or not??
            }
        }
    }
    return curr_len;
}

int loop2opt(std::vector<int> *path, std::vector<std::vector<double> > *dist, int curr_len) {
    int best_gain = 0;
    int loops = 100;
    int new_len;
    do {
        new_len = step_2OPT(path, dist, curr_len);
        // stop if no new improvement is achieved
        if(new_len == curr_len) {
            // std::cout << "stopped at" << 100 - loops << std::endl;
            break;
        }
        curr_len = new_len;
        // std::cout << best_gain <<std::endl;
        loops--;
    }
    while(loops > 0);

    // std::cout << "2OPT result: "<< new_len <<std::endl;
    return new_len;
}

#endif
