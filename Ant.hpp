class Ant {
 public:
    Bitmap visited;
    std::vector<int> tour;
    double tour_len;
    Ant(int n_cities): visited(n_cities), tour(n_cities) {}
};