#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <bitset>
#include <stdlib.h>
#include <random>
#include <float.h>

#define ALPHA 1.0
#define BETA 2.0
#define INITIAL_TAU 0.0001
#define EVAPORATE 0.95
#define Q 0.2

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

    std::getline(file, line, ' ');
    int n;
    file >> n;

    std::cout << n <<std::endl;
    for(int i=0;i<4;++i){
        std::getline(file, line, '\n');
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
            double dist = sqrt(((x_i-x_j)*(x_i-x_j))+((y_i-y_j)*(y_i-y_j)));
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

int main(int argc, const char ** argv) {
    // assumed param is given, no checks
    std::vector<std::vector<double> > *mat = get_matrix(argv[1]);
    int n = (*mat).size();
    std::vector<std::vector<double> > phero = std::vector<std::vector<double> >(n, std::vector<double> (n, INITIAL_TAU));

    // initialize ants
    int n_ants = n;
    std::vector<Ant *> ants(n_ants);
    for(int a=0; a<n_ants; ++a) {
        ants[a] = new Ant(n);
    }

    int n_iterations = 200;

    // Bitmap visited(n);
    std::vector<double> CDF(n);
    srand(time(0));

    // initialize uniform distribution between 0 and 1
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<float> distr(0, 1);

    double best_solution = DBL_MAX;

    for (int i=0; i<n_iterations; ++i) { // for each iteration of the program
        for(int a=0; a<n_ants; ++a) { // for each ant, make a tour
            // std::vector<int> tour(n);
            // double tour_len = 0.0;
            ants[a]->visited.clear();
            ants[a]->tour_len = 0;
            int curr_c = rand() % 100; // random first city for ant 'a'
            ants[a]->tour[0] = curr_c;

            // chose next city
            for(int c=1; c<n; c++) {// c++, hehe
                setCDF(&CDF, curr_c, &phero, mat, &(ants[a]->visited));
                int next_city = 0;
                double rand_0_1 = distr(eng);
                // loop the CDF and find the first index where the randomly generated number is greater-equal. That will be the next city.
                for(int prob_i=0; prob_i < n; prob_i++) {
                    // TODO check range of rand_0_1 and CDF, maybe 1 is not included in rand_0_1 but it is in CDF
                    if (rand_0_1 <= CDF[prob_i]) {
                        // update tour and visited with the newly selected city
                        next_city = prob_i;
                        ants[a]->tour[c] = next_city;
                        ants[a]->tour_len += (*mat)[curr_c][next_city];
                        curr_c = next_city;
                        ants[a]->visited.set(curr_c, 1);
                        break;
                    }
                }


            }
            // add last connection to the tour len
            ants[a]->tour_len += (*mat)[ants[a]->tour[0]][ants[a]->tour[n-1]];


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
    // TODO: cleanup dyn. allocated memory
}