#include "knn.h"

double stod(std::string s) { return std::stod(s); }

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Invalid usage: " << argv[0] << " \e[31;1m<k-value>\e[0m" << std::endl;
        exit(1);
    }

    auto names = misc::array<std::string>("euclidean_output.csv", "chebyshev_output.csv", "manhattan_output.csv");
    auto distances = misc::array<double (*)(const DataPoint<misc::array<double>>*, const DataPoint<misc::array<double>>*)>(distances::euclidean_distance, distances::chebyshev_distance, distances::manhattan_distance);
    knn::all_distances<double, double>("classified.csv", "Unclassified.csv", atoi(argv[1]), stod, names, distances);

    exit(0);
}

