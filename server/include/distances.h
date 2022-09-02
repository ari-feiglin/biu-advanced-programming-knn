#pragma once

#include <cmath>
#include "knn.h"

typedef knn::DataSet<misc::array<double>> dubdset;
typedef knn::DataPoint<misc::array<double>> dubdpoint;

namespace distances {
    /**
     * Returns the euclidean distance between the two points.
     * @param p1 first point.
     * @param p2 second point.
     * @return double the euclidean distance between the two points.
     */
    double euclidean_distance(const dubdpoint* p1, const dubdpoint* p2);

    /**
     * Returns the chebyshev distance between the two points.
     * @param p1 first point.
     * @param p2 second point.
     * @return double the chebyshev distance between the two points.
     */
    double chebyshev_distance(const dubdpoint* p1, const dubdpoint* p2);

    /**
     * Returns the manhattan distance between the two points.
     * @param p1 first point.
     * @param p2 second point.
     * @return double the manhattan distance between the two points.
     */
    double manhattan_distance(const dubdpoint* p1, const dubdpoint* p2);
}

