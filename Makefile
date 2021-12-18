CFLAGS = -std=c++11 -O3
SEED = 1092841564

compile:
	c++ $(CFLAGS) aco.cpp -o aco

runall:
	./aco ./problems_cleaned/pr439.tsp $(SEED)
	sleep 3
	./aco ./problems_cleaned/ch130.tsp $(SEED)
	sleep 3
	./aco ./problems_cleaned/eil76.tsp $(SEED)
	sleep 3
	./aco ./problems_cleaned/d198.tsp $(SEED)