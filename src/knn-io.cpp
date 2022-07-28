#include <string>
#include <fstream>
#include <sstream>
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
CartDataPoint<T> read_point(std::ifstream& input_stream, T (*converter)(std::string), bool classified=true);

/**
 * Initializes a Data Set from an input file stream.
 * @param input_stream      The input stream.
 * @return                  A Data Set of Cartesian Data Points read from the stream.
 */
template<typename T>
DataSet<misc::array<T>>& initialize_dataset(std::ifstream& input_stream, T (*converter)(std::string));

/**
 * Writes the name of the closest class (among the k closest neighbors) to a point to an input file stream.
 * @param data_set          The Data Set to use for finding neighbors.
 * @param k                 The k to use in quickselect.
 * @param p                 The Data Point to find the closest class to.
 * @param distance          A distance function for Data Points.
 * @param output_stream     The output file stream.     
 */
template <typename T, typename M>
void write_closest_class(const DataSet<CartDataPoint<T>>& data_set, int k, const CartDataPoint<T> p,
        M (*distance)(const DataPoint<misc::array<T>>&, const DataPoint<misc::array<T>>&),
        std::ofstream& output_stream);

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
void write_closest_classes(const DataSet<CartDataPoint<T>>& data_set, int k,
        M (*distance)(const DataPoint<misc::array<T>>&, const DataPoint<misc::array<T>>&),
        std::ifstream& input_stream, std::ofstream& output_stream,
        T (*converter)(std::string));

/**************************************************************************************************************************/

template <typename T>
CartDataPoint<T> read_point(std::ifstream& input_stream, T (*converter)(std::string), bool classified) {
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
        return CartDataPoint<T>(arr, curr_str);  
    } else {
        data.push_back(converter(curr_str));
        misc::array<T> arr(data);
        return CartDataPoint<T>(arr);
    }
}

template <typename T>
DataSet<misc::array<T>>& initialize_dataset(std::ifstream& input_stream, T (*converter)(std::string)) {
    DataSet<misc::array<T>>* dataset = new DataSet<misc::array<T>>();
    CartDataPoint<T> point;
    
    while ((point = read_point(input_stream, converter, true)) != nullptr) {
        dataset->add(point);
    }

    return *dataset;
}

template <typename T, typename M>
void write_closest_class(const DataSet<CartDataPoint<T>>& data_set, int k, const CartDataPoint<T> p,
        M (*distance)(const DataPoint<misc::array<T>>&, const DataPoint<misc::array<T>>&),
        std::ofstream& output_stream) {
    output_stream << data_set.get_nearest_class(k, p, distance) << std::endl;
}

template <typename T, typename M>
void write_closest_classes(const DataSet<CartDataPoint<T>>& data_set, int k,
        M (*distance)(const DataPoint<misc::array<T>>&, const DataPoint<misc::array<T>>&),
        std::ifstream& input_stream, std::ofstream& output_stream,
        T (*converter)(std::string)) {
    CartDataPoint<T> p;

    while ((p = read_point(input_stream, converter, false)) != nullptr) {
        write_closest_class(data_set, k, p, distance, output_stream);
    }
}

template <typename T, typename M>
void all_distances(std::string classified, std::string unclassified, int k,
        T (*converter)(std::string), misc::array<std::string> output_names,
        misc::array<M (*)(const DataPoint<misc::array<T>>&, const DataPoint<misc::array<T>>&)> distances) {
    std::ifstream classified_stream;
    classified_stream.open(classified);

    DataSet<misc::array<T>> dataset = initialize_dataset(classified_stream, converter);
    
    for (int i = 0; i < output_names.length(); i++) {
        std::ofstream output_stream;
        output_stream.open(output_names[i]);
        std::ifstream unclassified_stream;
        unclassified_stream.open(unclassified);

        write_closest_classes(dataset, k, distances[i], unclassified_stream, output_stream, converter);

        output_stream.close();
        unclassified_stream.close();
    }
}

