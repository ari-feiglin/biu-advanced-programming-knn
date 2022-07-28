#include "distances.h"
#include "misc.h"
#include "knn.h"
#include <cmath>


namespace distances {
    
    double euclidean_distance(const knn::DataPoint<misc::array<double>>& p1, const knn::DataPoint<misc::array<double>>& p2) {
        misc::array<double> p1Data = p1.data();
        misc::array<double> p2Data = p2.data();
        double retVal = 0.0;
        for(int i = 0; i < 4; i++){ //we know there are only 4 values of data in the arrays.
            retVal += (p1Data[i] - p2Data[i]) * (p1Data[i] - p2Data[i]);
        } 
        retVal = sqrt(retVal);
        return retVal;
    }

    double chebyshev_distance(const knn::DataPoint<misc::array<double>>& p1, const knn::DataPoint<misc::array<double>>& p2) {
        misc::array<double> p1Data = p1.data();
        misc::array<double> p2Data = p2.data();
        double maxDis = 0.0;
        for(int i = 0; i < 4; i++){ //we know there are only 4 values of data in the arrays.
            maxDis = max(maxDis, abs(p1Data[i] - p2Data[i]));
        } 
        return maxDis;
    }

    double manhattan_distance(const knn::DataPoint<misc::array<double>>& p1, const knn::DataPoint<misc::array<double>>& p2) {
        misc::array<double> p1Data = p1.data();
        misc::array<double> p2Data = p2.data();
        double retVal = 0.0;
        for(int i = 0; i < 4; i++){ //we know there are only 4 values of data in the arrays.
            retVal += abs(p1Data[i] - p2Data[i]);
        } 
        return retVal;
    }
}
