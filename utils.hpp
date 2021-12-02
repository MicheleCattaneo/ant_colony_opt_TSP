#include <fstream>
#include <cmath>

std::vector<std::vector<double> > * get_matrix(const char * file_path) {
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
            // print best known
            std::cout << line << std::endl;
        }
    }
    std::vector<std::vector<double> > *mat = new std::vector<std::vector<double> >(n, std::vector<double> (n, 0));
    std::vector<std::vector<double> > pos(n, std::vector<double> (2, 0));
    int i;
    double x,y;
    while(file >> i >> x >> y){
        pos[i-1][0] = x;
        pos[i-1][1] = y;
    }

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