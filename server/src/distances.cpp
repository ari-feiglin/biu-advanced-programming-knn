#include "distances.h"
#include <cmath>

namespace distances {
    
    double euclidean_distance(const dubdpoint* p1, const dubdpoint* p2) {
        misc::array<double> p1Data = p1->data();
        misc::array<double> p2Data = p2->data();
        double retVal = 0.0;

        p1Data.assert_comparable(p2Data);       // Check that both arrays are of a comparable length.

        for (size_t i = 0; i < p1Data.length(); i++) {
            retVal += (p1Data[i] - p2Data[i]) * (p1Data[i] - p2Data[i]);
        }

        return retVal;  // Don't need to take sqrt since sqrt(x) < sqrt(y) iff x < y
    }

    double chebyshev_distance(const dubdpoint* p1, const dubdpoint* p2) {
        misc::array<double> p1Data = p1->data();
        misc::array<double> p2Data = p2->data();
        double maxDis = 0.0;

        p1Data.assert_comparable(p2Data);       // Check that both arrays are of a comparable length.

        for (size_t i = 0; i < p1Data.length(); i++) {
            maxDis = std::max(maxDis, std::abs(p1Data[i] - p2Data[i]));
        }

        return maxDis;
    }

    double manhattan_distance(const dubdpoint* p1, const dubdpoint* p2) {
        misc::array<double> p1Data = p1->data();
        misc::array<double> p2Data = p2->data();
        double retVal = 0.0;

        p1Data.assert_comparable(p2Data);       // Check that both arrays are of a comparable length.

        for (size_t i = 0; i < p1Data.length(); i++) {
            retVal += std::abs(p1Data[i] - p2Data[i]);
        }

        return retVal;
    }
}

