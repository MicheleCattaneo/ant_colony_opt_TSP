import numpy as np

class ProblemInstance:
    
    def __init__(self, name_tsp):
        self.exist_opt = False
        self.optimal_tour = None
        self.dist_matrix = None

        # read raw data
        self.file_name = name_tsp
        file_object = open(self.file_name)
        data = file_object.read()
        file_object.close()
        self.lines = data.splitlines()

        # store data set information
        # ciao brites
        self.name = self.lines[0].split(': ')[1]
        self.nPoints = np.int(self.lines[3].split(': ')[1])
        self.best_sol = np.float(self.lines[5].split(': ')[1])

        # read all data points and store them
        self.points = np.zeros((self.nPoints, 3))
        for i in range(self.nPoints):
            
            line_i = self.lines[7 + i].split(' ')
#             print(line_i)
            self.points[i, 0] = int(line_i[0])
            self.points[i, 1] = line_i[1]
            self.points[i, 2] = line_i[2]
        self.create_dist_matrix();
            
    def create_dist_matrix(self):
        self.dist_matrix = np.zeros((self.nPoints, self.nPoints))

        for i in range(self.nPoints):
            for j in range(i, self.nPoints):
                self.dist_matrix[i, j] = distance_euc(self.points[i][1:3], self.points[j][1:3])
        self.dist_matrix += self.dist_matrix.T
        
    def GAP(self, found):
        return np.round(
            ((found - self.best_sol) / self.best_sol) * 100, 3)

    def ValidateSolution(self, path):
        return min(path) == 0 and max(path) == self.nPoints-1 and len(path) == len(set(path)) \
        and sum(path) == sum(i for i in range(self.nPoints))


def compute_length(solution, dist_matrix):
    total_length = 0
    starting_node = solution[0]
    from_node = starting_node
    for node in solution[1:]:
        total_length += dist_matrix[from_node, node]
        from_node = node
    total_length += dist_matrix[starting_node, from_node]
    return total_length


def distance_euc(point_i, point_j):
    rounding = 0
    x_i, y_i = point_i[0], point_i[1]
    x_j, y_j = point_j[0], point_j[1]
    distance = np.sqrt((x_i - x_j) ** 2 + (y_i - y_j) ** 2)
    return round(distance, rounding)