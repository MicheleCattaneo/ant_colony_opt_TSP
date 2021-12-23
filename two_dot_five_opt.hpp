#ifndef TWODOTFIVEOPT
#define TWODOTFIVEOPT

#include <vector>
#include <iostream>
#include <algorithm>

int gain2(int i, int j, std::vector<std::vector<double> > *dist, std::vector<int> *path) {
    int path_iminus1 = (*path)[i - 1];
    int path_jplus1 = (*path)[j+1];
    int path_i = (*path)[i];
    int path_j = (*path)[j];
    int old_link_len = (*dist)[path_i][path_iminus1] + (*dist)[path_j][path_jplus1];
    int changed_links_len = (*dist)[path_j][path_iminus1] + (*dist)[path_i][path_jplus1];

    return -old_link_len + changed_links_len;
}
void swap2opt2(std::vector<int> *path, int i, int j) {
    std::reverse(path->begin()+i, path->begin()+j+1);
}


void shift1(std::vector<int> *path, int i, int j) {
    std::vector<int> temp = (*path);
    int write_from = i;
    for(int k=i+1; k < j+1; k++) { // tsp_sequence[:i]
        (*path)[write_from++]=temp[k];
    }
    (*path)[write_from++] = temp[i]; // tsp_sequence[i]
    // write_from++;
    for(int k=j+1; k< temp.size(); k++) {
        (*path)[write_from++] = temp[k];
    }
}

int shift_gain1(int i, int j, std::vector<std::vector<double> > *dist, std::vector<int> *path) {
    int pathi = (*path)[i];
    int pathj = (*path)[j];
    int pahtiminus1 = (*path)[i-1];
    int pathiplus1 = (*path)[i+1];
    int pathjplus1 = (*path)[j+1];
    int old_link_len =(*dist)[pathi][pahtiminus1] + (*dist)[pathi][pathiplus1] + (*dist)[pathj][pathjplus1];
    int changed_links_len = (*dist)[pahtiminus1][pathiplus1] + (*dist)[pathi][pathj] + (*dist)[pathi][pathjplus1];
    return -old_link_len + changed_links_len;
}


void shift2(std::vector<int> *path, int i, int j) {
    std::vector<int> temp = (*path);
    int write_from = i;
    (*path)[write_from++] = temp[j];
    for(int k=i; k<j;k++) {
        (*path)[write_from++] = temp[k];
    }
    for(int k=j+1; k<temp.size();k++) {
        (*path)[write_from++] = temp[k];
    }
}

int shift_gain2(int i, int j, std::vector<std::vector<double> > *dist, std::vector<int> *path) {
    int pathi = (*path)[i];
    int pathiminus1 = (*path)[i-1];
    int pathjminus1 = (*path)[j-1];
    int pathjplus1 = (*path)[j+1];
    int pathj = (*path)[j];
    int old_link_len = (*dist)[pathi][pathiminus1] + (*dist)[pathj][pathjminus1] + (*dist)[pathj][pathjplus1];
    int changed_links_len = (*dist)[pathj][pathiminus1] + (*dist)[pathi][pathj] + (*dist)[pathjminus1][pathjplus1];
    return -old_link_len + changed_links_len;
}

int step_2DOT5OPT(std::vector<int> *path, std::vector<std::vector<double> > *dist, int curr_len) {
    int n = (*path).size()-2;
    int best_i = -1;
    int best_j = -1;
    int best_gain = 0;
    int best_new_distance = 0;
    void (*best_func)(std::vector<int> *, int,int);

    for(int i=1; i < n -1; i++) {
        for(int j=i+1; j < n; j++) {
            int two_opt_g = gain2(i,j,dist, path);
            int shift1_g = shift_gain1(i,j, dist, path);
            int shift2_g = shift_gain2(i,j, dist,path);
            
            if (two_opt_g < shift1_g && two_opt_g < shift2_g){
                int new_distance = curr_len + two_opt_g;
                if (new_distance < curr_len) {
                    swap2opt2(path, i ,j);
                    curr_len = new_distance;
                }
            }
            else if (shift1_g < two_opt_g && shift1_g < shift2_g) {
                int new_distance = curr_len + shift1_g;
                if (new_distance < curr_len) {
                    shift1(path, i ,j);
                    curr_len = new_distance;
                }
            } 
            else {
                int new_distance = curr_len + shift2_g;
                if (new_distance < curr_len) {
                    shift2(path, i ,j);
                    curr_len = new_distance;
                }
            }
        }
    }
    return curr_len;
    
}


int loop25opt(std::vector<int> *path, std::vector<std::vector<double> > *dist, int curr_len) {
    int best_gain = 0;
    int loops = 100;
    int new_len;
    do {
        new_len = step_2DOT5OPT(path, dist, curr_len);
        // stop if no new improvement is achieved
        if(new_len == curr_len) {
            break;
        }
        curr_len = new_len;
        loops--;
    }
    while(loops > 0);
    return new_len;
}

// int main() {

// }

#endif