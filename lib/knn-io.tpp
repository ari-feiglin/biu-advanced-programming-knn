#pragma once

#include <iostream>
#include "knn.h"

namespace knn {
    template <typename T>
    CartDataPoint<T>* get_point(std::string str, T (*converter)(std::string), bool classified) {
        size_t prev_index = str.find(',');
        size_t curr_index = 0;
        std::vector<T> data;

        data.push_back(converter(str.substr(0, prev_index)));

        while ((curr_index = str.find(',', prev_index + 1)) != std::string::npos) {
            data.push_back(converter(str.substr(prev_index + 1, curr_index - prev_index - 1)));
            prev_index = curr_index;
        }

        if (classified) {
            misc::array<T> arr(data);
            return new CartDataPoint<T>(str.substr(prev_index + 1), arr);
        }

        data.push_back(converter(str.substr(prev_index + 1)));
        misc::array<T> arr(data);
        return new CartDataPoint<T>(arr);
    }

    template <typename T>
    CartDataPoint<T>* read_point(std::function<std::string(std::string&)> getline, T (*converter)(std::string), bool classified) {
        std::vector<T> data;
        std::string line;
    
        if(getline(line) == "") return nullptr;
    
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
    
    template <typename T>
    DataSet<misc::array<T>>* initialize_dataset(std::function<std::string(std::string&)> getline, T (*converter)(std::string)) {
        DataSet<misc::array<T>>* dataset = new DataSet<misc::array<T>>();
        CartDataPoint<T>* point;
        
        while ((point = read_point<T>(getline, converter, true)) != nullptr) {
            dataset->add(point);
            delete point;
        }
    
        return dataset;
    }
}

namespace {
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
} // anonymous

namespace knn {
    template <typename T, typename M>
    void all_distances(std::string classified, std::string unclassified, int k,
            T (*converter)(std::string), misc::array<std::string> output_names,
            misc::array<M (*)(const DataPoint<misc::array<T>>*, const DataPoint<misc::array<T>>*)> distances) {
        output_names.assert_comparable(distances);

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
}

