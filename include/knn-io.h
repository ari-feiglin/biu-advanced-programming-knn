#pragma once

#include "knn.h"

namespace knn {
    /**
     * Classifies points relative to different distances.
     * @param classified            File name of classified points to initialize a Data Set with.
     * @param unclassified          File name of unclassified points to classify.
     * @param k                     The k to use in quickselect.
     * @param converter             Converter from string to correct datatype (T).
     * @param output_names          Names of output files.
     * @param distances             Array of distance functions.
     */
    template <typename T, typename M>
    void all_distances(std::string classified, std::string unclassified, int k,
            T (*converter)(std::string), misc::array<std::string> output_names,
            misc::array<M (*)(const DataPoint<misc::array<T>>*, const DataPoint<misc::array<T>>*)> distances);

    /**
     * Reads in a Cartesian Data Point from an input file stream.
     * @param input_stream      The input stream.
     * @param converter         Function to convert strings to the correct datatype.
     * @param classified        If the Data Points in the file have classes.
     * @return                  A Cartesian Data Point read from the stream.
     *                          Returns null upon reaching the end of the stream.
     */
    template <typename T>
    CartDataPoint<T>* read_point(std::ifstream& input_stream, T (*converter)(std::string), bool classified);

    /**
     * Initializes a Data Set from an input file stream.
     * @param input_stream      The input stream.
     * @return                  A Data Set of Cartesian Data Points read from the stream.
     */
    template <typename T>
    DataSet<misc::array<T>>* initialize_dataset(std::ifstream& input_stream, T (*converter)(std::string));
}

#include "knn-io.tpp"

