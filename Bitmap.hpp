#ifndef BITMAP
#define BITMAP

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

#endif