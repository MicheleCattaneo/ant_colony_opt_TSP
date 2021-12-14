#ifndef ANT
#define ANT
#include <vector>

class Ant {
 public:
    Bitmap visited;
    std::vector<int> tour;
    double tour_len;
    Ant(int n_cities): visited(n_cities), tour(n_cities) {}
};

void cleanUpAnts(std::vector<Ant *> ants) {
    for(int i=0;i<ants.size(); ++i) {
        delete ants[i];
    }
}
#endif