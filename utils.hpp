#ifndef UTILS
#define UTILS
#include <fstream>
#include <cmath>
#include <sstream>
#include <string>

std::vector<std::vector<double> > * get_matrix(const char * file_path, int * best_) {
    std::ifstream file(file_path);
    std::string line;
    // skip 3
    for(int i=0;i<3;++i){
        std::getline(file, line, '\n');
    }

    std::getline(file, line, ':');
    int n;
    file >> n;

    std::cout << n <<std::endl;
    for(int i=0;i<2;++i){
        std::getline(file, line, '\n');
    }
    for(int i=0;i<2;++i){
        std::getline(file, line, '\n');
        if(i==0){
            std::stringstream ss;    
            ss << line;
            std::string temp;
            int found;
            while (!ss.eof()) {
                ss >> temp;
                if (std::stringstream(temp) >> found)
                    // std::cout << found << " ";
        
                temp = "";
            }
            std::cout << "BEST KNOWN IS " << found << std::endl;
            *best_ = found;
        }
    }
    std::vector<std::vector<double> > *mat = new std::vector<std::vector<double> >(n, std::vector<double> (n, 0));
    std::vector<std::vector<double> > pos(n, std::vector<double> (2, 0));
    int i;
    double x,y;
    int count = 0;
    while(file >> i >> x >> y){
        pos[i-1][0] = x;
        pos[i-1][1] = y;
        count ++;
    }
    std::cout << count << "==" << n << std::endl;

    double x_i, y_i, x_j, y_j;
    for (int i=0;i<n;++i) {
        for(int j=0; j<n;++j) {
            x_i = pos[i][0];
            y_i = pos[i][1];
            x_j = pos[j][0];
            y_j = pos[j][1];
            
            // TODO, only keep half matrix? lower -> j < i
            double dist = round(sqrt(((x_i-x_j)*(x_i-x_j))+((y_i-y_j)*(y_i-y_j))));
            (*mat)[i][j] = dist;
            (*mat)[j][i] = dist;
        }
    }
    return mat;
}

void cleanUpMatrix( std::vector<std::vector<double> > *mat) {
    delete mat;
}

float gap(int found_len, int best_known){

    return  (float((found_len - best_known)) / float(best_known)) * 100.0f;
}

int compute_path_len(std::vector<std::vector<double> > * mat, std::vector<int> *path ) {
    int s = 0;
    int n = path->size();
    for(int i=0; i<n-1; ++i) {
        s += (*mat)[(*path).at(i)][(*path).at(i+1)];
    }
    s+= (*mat)[0][(*path)[n-1]];
    std::cout << "\n SIZE IS " << s << std::endl; // TODO FIXXX
    return s;
}

#endif