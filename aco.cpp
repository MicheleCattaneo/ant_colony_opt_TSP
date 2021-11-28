#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <bitset>
#include <stdlib.h>
#include <random>
#include <float.h>

#include "2opt.cpp"

// How mow to exploit the cumulated knowledge
#define ALPHA 1.0
// How much to explore based on distances
#define BETA 2.0
#define INITIAL_TAU 0.0018
#define EVAPORATE 0.95
#define Q 1.0
#define Q_0 0.90

/**
 * This class allows for a dynamic bitmap with optimized space.
 * Every 64 bits are represented by an unsigned long.
 * */
class Bitmap {
    unsigned long * values;
    size_t size;
  public:
    Bitmap( int size): size(size) {
        this->values = new unsigned long[size/64 + (size % 64 != 0)];
        for(int i=0;i<size/64 + (size % 64 != 0);++i) {
            this->values[i] = 0;
        }
    }

    ~Bitmap() {
        delete(this->values);
    }

    /**
     * Set the i-th bit to val
     */ 
    void set(int i, bool val) {
        int block = i/64;
        int cell = i%64;
        // std::cout << "block " << block << ' ' << "cell " << cell << std::endl;
        // number |= 1UL << n;
        this->values[block] |= 1UL << cell;
    }
    /**
     * Get the i-th bit
     */ 
    bool get(int i) {
        int block = i/64;
        int cell = i%64;
        return (this->values[block] >> cell) & 1U;
    }
    /**
     *  Set all bits to false 
     */
    void clear() {
        for(int i=0;i<size/64 + (size % 64 != 0);++i) {
            this->values[i] = 0;
        }
    }
};


class Ant {
 public:
    Bitmap visited;
    std::vector<int> tour;
    double tour_len;
    Ant(int n_cities): visited(n_cities), tour(n_cities) {}
};

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

/**
 * Sets the probabilities of selecting new city as a Cumulative distribution function.
 * @param probs the pointer to the vector that will contain the values
 * @param i the starting city
 * @param phero the pointer to the pheromone matrix
 * @param dist the pointer to the distance matrix
 * @param visited the pointer to the bitmap to know unvisited cities
 */
void setCDF(std::vector<double> *probs, int i, std::vector<std::vector<double> > *phero, std::vector<std::vector<double> > *dist, Bitmap * visited) {
    int n = (*dist).size();
    // get denominator
    double tau_tot = 0.0;
    for (int k=0;k<n;++k) {
        if ( (*visited).get(k) == 0 && i != k) {
            tau_tot += pow((*phero)[i][k], ALPHA) * pow((1.0/(*dist)[i][k]), BETA);
        }
    }
    // build the CDF
    double prev = 0.0;
    for(int j=0;j<n;++j) {
        if ( (*visited).get(j) == 0 && i != j) {// if not visited
            (*probs)[j] = prev + (pow((*phero)[i][j], ALPHA) * pow((1.0/(*dist)[i][j]), BETA))/tau_tot;
            prev = (*probs)[j];
        } else {
            (*probs)[j] = prev;
        }
    }
}

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

int get_next_city(int i, double prob, std::vector<std::vector<double> > *phero, std::vector<std::vector<double> > *dist,  Bitmap * visited) {
    int n = (*dist).size();
    // get denominator
    double tau_tot = 0.0;
    for (int k=0;k<n;++k) {
        if ( (*visited).get(k) == 0 && i != k) {
            tau_tot += pow((*phero)[i][k], ALPHA) * pow((1.0/(*dist)[i][k]), BETA);
        }
    }

    double prev = 0.0;
    double curr_prob = 0.0;
    for(int j=0;j<n;++j) {
        if ( (*visited).get(j) == 0 && i != j) {// if not visited
            curr_prob = prev + (pow((*phero)[i][j], ALPHA) * pow((1.0/(*dist)[i][j]), BETA))/tau_tot;
            if (prob <= curr_prob) {
                return j;
            }
            prev = curr_prob;
        }
        //else {
        //     (*probs)[j] = prev;
        // }
    }
    std::cout << "ciao" << std::endl;
    return n-1;
}

// /**
//  * Search for the first value x in a cumulative distribution function such that x >= val.
//  * @param CDF a vector of probabilities representing a cumulative distribution function
//  * @param val the value we are looking for.
//  * TODO: TESTME.
//  */
// int binarySearchCDF(std::vector<double> CDF, double val) {
// int l = 0;
//     int r = CDF.size()-1;
//     int m;
//     for (;l<=r;) {
//         if (l==r) {
//             return r;
//         }

//         m = (l+r)/2;
//         if (CDF[m] == val) {
//             return m;
//         } 
//         else if (CDF[m] < val) {
//             l = m+1;
//         } 
//         else {
//             r = m;
    //     }
    // }

    // return -1;}


int main(int argc, const char ** argv) {
    unsigned seed = 123456;
    if(argc == 3) {
        seed = atoi(argv[2]);
        std::cout << "seed" << seed << std::endl;
    }
    // assumed param is given, no checks
    std::vector<std::vector<double> > *mat = get_matrix(argv[1]);
    int n = (*mat).size();
    std::vector<std::vector<double> > phero = std::vector<std::vector<double> >(n, std::vector<double> (n, INITIAL_TAU));

    // initialize ants
    // int n_ants = n < 200 ? n: 50; // TODO fix this values
    int n_ants = 10;
    std::vector<Ant *> ants(n_ants);
    for(int a=0; a<n_ants; ++a) {
        ants[a] = new Ant(n);
    }

    int n_iterations = 200;

    // Bitmap visited(n);
    std::vector<double> CDF(n);
    // srand(time(0));

    // initialize uniform distribution between 0 and 1
    // std::random_device rd;  // Will be used to obtain a seed for the random number engine
    // std::mt19937 eng(rd());
    // std::random_device rd;
    
    std::default_random_engine generator (seed);
    std::uniform_real_distribution<double> distr(0, 1);

    double best_solution = DBL_MAX;
    std::vector<int> best_tour;

    for (int i=0; i<n_iterations; ++i) { // for each iteration of the program
        // std::cout << i << std::endl;
        for(int a=0; a<n_ants; ++a) { // for each ant, make a tour
            // std::vector<int> tour(n);
            // double tour_len = 0.0;
            ants[a]->visited.clear();
            ants[a]->tour_len = 0;
            // int curr_c = rand() % n; // random first city for ant 'a'

            int curr_c = distr(generator) * n;

            // int curr_c = int(distr(eng)*n);
            ants[a]->tour[0] = curr_c;
            ants[a]->visited.set(curr_c,1);// right?

            // chose next city
            for(int c=1; c<n; c++) {// c++, hehe

                // Possible bottleneck here, avoid complete computation?
                // setCDF(&CDF, curr_c, &phero, mat, &(ants[a]->visited));
                int next_city = 0;
                double rand_0_1 = distr(generator);

                //if Q < Q0 -> Exploitation
                if (rand_0_1 < Q_0) {
                    next_city = get_next_best_city(curr_c, &phero, mat, &(ants[a]->visited));
                } else { // else Exploration
                    rand_0_1 = distr(generator);
                    next_city = get_next_city(curr_c, rand_0_1, &phero, mat, &(ants[a]->visited));
                }
                // loop the CDF and find the first index where the randomly generated number is greater-equal. That will be the next city.

                // next_city = binarySearchCDF(CDF, rand_0_1);
                ants[a]->tour[c] = next_city;
                ants[a]->tour_len += (*mat)[curr_c][next_city];
                curr_c = next_city;
                ants[a]->visited.set(curr_c, 1);

            }
            // add last connection to the tour len
            ants[a]->tour_len += (*mat)[ants[a]->tour[0]][ants[a]->tour[n-1]];
            // std::cout << ants[a]->tour[0] << ' ' << ants[a]->tour[n-1] << std::end;


        }
        // decay
        for(int i=0;i<n;++i) {
            for(int j=0; j<=i; ++j) {
                double curr_val = phero[i][j]*EVAPORATE;
                phero[i][j] = curr_val;
                phero[j][i] = curr_val;
            }
        }
        // add pheromone on used paths
        for (int a=0; a<n_ants; ++a) {
            double curr_tour_len = ants[a]->tour_len;
            if(curr_tour_len < best_solution) {
                best_solution = curr_tour_len;
                // save best solution
                best_tour = ants[a]->tour;
                // std::copy(ants[a]->tour.begin(), ants[a]->tour.end(), back_inserter(best_tour));
            }
            for(int indx = 1; indx < n; indx++) {
                int i = ants[a]->tour[indx-1];
                int j = ants[a]->tour[indx];
                double curr_val = phero[i][j];
                double new_val = curr_val + (Q/curr_tour_len);
                phero[i][j] = new_val;
                phero[j][i] = new_val;
            }
        }
    }
    std::cout << "Best solution: " << best_solution << std::endl;
    for(auto itr = best_tour.begin(); itr != best_tour.end(); itr++) {
        std::cout << *itr << ',';
    }
    std::cout << std::endl;
    // TODO: cleanup dyn. allocated memory
}