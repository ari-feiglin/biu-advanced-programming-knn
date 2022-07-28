#include "knn.h"
#include <cstdlib>

using namespace knn;

double stod(std::string s) { return std::stod(s); }

int main(int argc, char** argv) {
    auto names = misc::array<std::string>("euclidian.csv", "chebyshev.csv", "manhattan.csv");
    auto distances = misc::array<double (*)(const DataPoint<misc::array<double>>&, const DataPoint<misc::array<double>>&)>(distances::euclidean_distance, distances::chebyshev_distance, distances::manhattan_distance);
    all_distances<double, double>("classified.csv", "Unclassified.csv", atoi(argv[1]), stod, names, distances);
            
            
}
