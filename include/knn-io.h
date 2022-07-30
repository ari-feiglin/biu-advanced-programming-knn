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
}

#include "knn-io.tpp"

