#pragma once

#include "knn.h"

namespace knn {
    /**
     * Interface class for Data Points which represent points of data in a Data Set.
     */
    template <typename T>
    class DataPoint {
        public:
            virtual ~DataPoint() {} // =0;

            /**
             * Get the name of the class of the data point (eg. if the dataset stores information
             * on flowers, a possible class name of a data point might be "rose".)
             * @return The name of the class of the data point.
             */
            virtual std::string class_type() const {} // =0;

            /**
             * Gets the data stored by the Data Point.
             * @return The data stored in the point.
             */
            virtual const T& data() const {}// =0;
    };

    /**
     * Class representing a literal data **point**.
     * Stores data as an array.
     */
    template <typename T>
    class CartDataPoint : public DataPoint<misc::array<T>> {
        const misc::array<T> m_data;
        std::string m_class_name;

        public:
            CartDataPoint() {}

            CartDataPoint(const CartDataPoint& other) :
                m_data(other.m_data), m_class_name(other.m_class_name) {}

            CartDataPoint(CartDataPoint&& other) {
                this->m_data = other.m_data;
                this->m_class_name = std::move(other.m_class_name);
            }

            CartDataPoint(const misc::array<T> data) :
                m_data(data), m_class_name() {}

            CartDataPoint(std::string class_name, const misc::array<T> data) :
                m_data(data), m_class_name(class_name) {}
            
            std::string class_type() const override { return this->m_class_name; }

            const misc::array<T>& data() const override { return this->m_data; }
    };

    /**
     * Class for storing a collection of data points and manipulating them.
     */
    template <typename T>
    class DataSet {
        std::vector<DataPoint<T>> m_data;

        public:
            /**
             * Default constructor for DataSet.
             */
            DataSet() {}

            /**
             * Constructors and assignment operators for the Data Set.
             * @param data      The collection of data points to add to the data set.
             * @param other     The data set to assign to this.
             */
            DataSet(const std::vector<DataPoint<T>>& data) : m_data(data) {}
            DataSet(std::vector<DataPoint<T>>&& data) : m_data(std::move(data)) {}

            /**
             * Add a Data Point to the Data Set.
             * @param data_point        The point to add to the set.
             * @return                  A reference to this Data Set.
             */
            DataSet& add(DataPoint<T> data_point);

            /**
             * Remove a Data Point to the Data Set.
             * @param index         The index of the Data Point to remove.
             * @return              A reference to this Data Set.
             */
            DataSet& remove(int index);

            /**
             * Gets the k-nearest neighbors to another input Data Point.
             * @param k             The k-value to run the algorithm on.
             * @param p             The point to find the nearest neighbors to.
             * @param distance      A function for computing distances between Data Points.
             * @return              An array whose first k elements are the closest neighbors to p.
             */
            template <typename M>
            DataPoint<T> * get_k_nearest(int k, const DataPoint<T>& p, M (*distance)(const DataPoint<T>&, const DataPoint<T>&)) const;

            /**
             * Gets the class name of the nearest class to a give input Data Point.
             * @param k             The k-value for the KNN algorithm.
             * @param p             The point to find neighbors relative to.
             * @param distance      A function for computing distances between Data Points.
             * @return              The name of the nearest class to p.
             */
            template <typename M>
            std::string get_nearest_class(int k, const DataPoint<T>& p, M (*distance)(const DataPoint<T>&, const DataPoint<T>&)) const;

        private:
            /**
             * Struct for representing distances between Data points.
             * Stores the distance and index of the Data Point.
             */
            template <typename M>
            struct DistancePoint {
                int index;
                M distance;

                DistancePoint(int i, M d) : index(i), distance(d) {}
                int operator<(DistancePoint<M> other) { return this->distance < other.distance; }
            };

            /**
             * Transforms the vector of Data Points to a vector of Distance Points.
             * @param p             The point to find the distance relative to.
             * @param distance      The distance function.
             * @return              A vector of all the distances.
             */
            template <typename M>
            std::vector<DistancePoint<M>> transform_data(const DataPoint<T>& p, M (*distance)(const DataPoint<T>&, const DataPoint<T>&)) const {
               std::vector<DistancePoint<M>> distances;
               int i = 0;

               for (auto it = this->m_data.begin(); it != this->m_data.end(); it++, i++) {
                   distances.push_back(DistancePoint<M>(i, distance(p, *it)));
               }

               return distances;
            }
    };

    /**
     * Classifies points relative to different distances.
     * @param classified            File name of classified points to initialize a Data Set with.
     * @param unclassified          File name of unclassified points to classify.
     * @param k                     The k to use in quickselect.
     * @param converter             Converter from string to correct datatype (T).
     * @param output_names          Names of output files.
     * @param distances             Array of distance functions.
     *//*
    template <typename T, typename M>
    void all_distances(std::string classified, std::string unclassified, int k,
            T (*converter)(std::string), misc::array<std::string> output_names,
            misc::array<M (*)(const DataPoint<misc::array<T>>&, const DataPoint<misc::array<T>>&)> distances);*/

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
    std::vector<DistancePoint<M>> selected_distances = quickselect<DistancePoint<M>>(this->transform_data(p, distance), k);
    DataPoint<T>* selected_points = new DataPoint<T>[k];
    int i = 0;

    for (auto it = selected_distances.begin(); it != selected_distances.end(); it++, i++) {
        selected_points[i] = this->m_data[it->index];
    }
    
    return selected_points;
}

template <typename T>
template <typename M>
std::string DataSet<T>::get_nearest_class(int k, const DataPoint<T>& p, M (*distance)(const DataPoint<T>&, const DataPoint<T>&)) const {
    std::unordered_map<std::string, int> classes;
    DataPoint<T> * selected_points = this->get_k_nearest(k, p, distance);

    for (int i = 0; i < k; i++) {
        //std::cout << "Looking for " << selected_points[i].class_type() << std::endl;
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

}

