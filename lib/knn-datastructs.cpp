#include <unordered_map>
#include "knn.h"

using namespace knn;

template <typename T>
DataSet<T>& DataSet<T>::add(DataPoint<T> data_point) {
    this->m_data.push_back(data_point);
    return *this;
}

template <typename T>
DataSet<T>& DataSet<T>::remove(int index) {
    this->m_data.erase(this->m_data.begin() + index);
    return *this;
}

template <typename T>
template <typename M>
DataPoint<T> * DataSet<T>::get_k_nearest(int k, const DataPoint<T>& p, M (*distance)(const DataPoint<T>&, const DataPoint<T>&)) const {
    std::vector<DistancePoint<M>> selected_distances = quickselect(this->transform_data(p, distance), k);
    DataPoint<T>* selected_points = new DataPoint<T>[k];
    int i = 0;

    for (DistancePoint<M>* it = selected_distances.begin(); it != selected_distances.end(); it++, i++) {
        selected_points[i] = this->m_data[it->index];
    }
    
    return *selected_points;
}

template <typename T>
template <typename M>
std::string DataSet<T>::get_nearest_class(int k, const DataPoint<T>& p, M (*distance)(const DataPoint<T>&, const DataPoint<T>&)) const {
    std::unordered_map<std::string, int> classes;
    DataPoint<T> * selected_points = this->get_k_nearest(k, p, distance);

    for (int i = 0; i < k; i++) {
        if (classes.find(selected_points[i].class_type()) == classes.end()) {
            classes[selected_points[i].class_type()] = 1;
        } else {
            classes[selected_points[i].class_type()]++;
        }
    }

    int max_count = 0;
    std::string max_string;

    for (auto entry : classes) {
        if (entry.second > max_count) {
            max_string = entry.first;
            max_count = entry.second;
        }
    }

    return max_string;
}

