#include <iostream>
#include "knn.h"

using namespace knn;

/**
 * Reads in a Cartesian Data Point from an input file stream.
 * @param input_stream      The input stream.
 * @param converter         Function to convert strings to the correct datatype.
 * @param classified        If the Data Points in the file have classes.
 * @return                  A Cartesian Data Point read from the stream.
 *                          Returns null upon reaching the end of the stream.
 */
template <typename T>
CartDataPoint<T>* read_point(std::ifstream& input_stream, T (*converter)(std::string), bool classified) {
    std::vector<T> data;
    std::string line;

    if(!std::getline(input_stream, line)) return nullptr;

    std::stringstream line_stream(line);
    
    std::string curr_str;
    std::string prev_str;

    std::getline(line_stream, prev_str, ',');
    
    while (std::getline(line_stream, curr_str, ',')) {
        data.push_back(converter(prev_str));
        prev_str = curr_str;
    }

    if (classified) {
        misc::array<T> arr(data);
        return new CartDataPoint<T>(curr_str, arr);
    } else {
        data.push_back(converter(curr_str));
        misc::array<T> arr(data);
        return new CartDataPoint<T>(arr);
    }
}

/**
 * Initializes a Data Set from an input file stream.
 * @param input_stream      The input stream.
 * @return                  A Data Set of Cartesian Data Points read from the stream.
 */
template <typename T>
DataSet<misc::array<T>>* initialize_dataset(std::ifstream& input_stream, T (*converter)(std::string)) {
    DataSet<misc::array<T>>* dataset = new DataSet<misc::array<T>>();
    CartDataPoint<T>* point;
    
    while ((point = read_point<T>(input_stream, converter, true)) != nullptr) {
        dataset->add(point);
        delete point;
    }

    return dataset;
}

/**
 * Writes the name of the closest class (among the k closest neighbors) to a point to an input file stream.
 * @param data_set          The Data Set to use for finding neighbors.
 * @param k                 The k to use in quickselect.
 * @param p                 The Data Point to find the closest class to.
 * @param distance          A distance function for Data Points.
 * @param output_stream     The output file stream.     
 */
template <typename T, typename M>
void write_closest_class(DataSet<misc::array<T>>& data_set, int k, const CartDataPoint<T>* p,
        M (*distance)(const DataPoint<misc::array<T>>*, const DataPoint<misc::array<T>>*),
        std::ofstream& output_stream) {
    output_stream << data_set.get_nearest_class(k, p, distance) << "\n";
}

/**
 * Writes the closest classes of a file of points to an output file.
 * @param data_set          The Data Set of classified points.
 * @param k                 The k to use in quickselect.
 * @param distance          Distance function for Data Points.
 * @param input_stream      File stream for unclassified points.
 * @param output_stream     The output file stream.
 * @param converter         Converter from string to correct data type.
 */
template <typename T, typename M>
void write_closest_classes(DataSet<misc::array<T>>& data_set, int k,
        M (*distance)(const DataPoint<misc::array<T>>*, const DataPoint<misc::array<T>>*),
        std::ifstream& input_stream, std::ofstream& output_stream,
        T (*converter)(std::string)) {
    CartDataPoint<T>* p;

    while ((p = read_point(input_stream, converter, false)) != nullptr) {
        write_closest_class<T,M>(data_set, k, p, distance, output_stream);
        delete p;
    }
}

template <typename T, typename M>
void all_distances(std::string classified, std::string unclassified, int k,
        T (*converter)(std::string), misc::array<std::string> output_names,
        misc::array<M (*)(const DataPoint<misc::array<T>>*, const DataPoint<misc::array<T>>*)> distances) {
    std::ifstream classified_stream;
    classified_stream.open(classified);

    DataSet<misc::array<T>>* dataset = initialize_dataset<T>(classified_stream, converter);
    
    for (int i = 0; i < output_names.length(); i++) {
        std::ofstream output_stream;
        output_stream.open(output_names[i]);
        std::ifstream unclassified_stream;
        unclassified_stream.open(unclassified);

        write_closest_classes<T,M>(*dataset, k, distances[i], unclassified_stream, output_stream, converter);

        output_stream.close();
        unclassified_stream.close();
    }

    delete dataset;
}

double stod(std::string s) { return std::stod(s); }

int main(int argc, char** argv) {
    auto names = misc::array<std::string>("euclidian.csv", "chebyshev.csv", "manhattan.csv");
    auto distances = misc::array<double (*)(const DataPoint<misc::array<double>>*, const DataPoint<misc::array<double>>*)>(distances::euclidean_distance, distances::chebyshev_distance, distances::manhattan_distance);
    all_distances<double, double>("classified.csv", "Unclassified.csv", atoi(argv[1]), stod, names, distances);
}

