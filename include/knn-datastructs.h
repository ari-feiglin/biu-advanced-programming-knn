#pragma once

#include "knn.h"

namespace knn {
    /**
     * Creates a vector of the k-smallest elements in an input vector.
     * @param vec           The vector to investigate.
     * @param k             The k value used in the algorithm.
     * @note                The function uses the Quick Select algorithm and thus has an average time
     *                      complexity of O(n).
     *                      Furthermore, the elements (T) must be comparable with the < operator.
     */
    template <typename T>
    std::vector<T> quickselect(const std::vector<T>& vec, int k);

    /**
     * Interface class for Data Points which represent points of data in a Data Set.
     */
    template <typename T>
    class DataPoint {
        public:
            virtual ~DataPoint() =0;

            /**
             * Get the name of the class of the data point (eg. if the dataset stores information
             * on flowers, a possible class name of a data point might be "rose".)
             * @return The name of the class of the data point.
             */
            virtual std::string class_type() const =0;

            /**
             * Gets the data stored by the Data Point.
             * @return The data stored in the point.
             */
            virtual const T& data() const =0;
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
            CartDataPoint(const misc::array<T> data) :
                m_data(data), m_class_name(nullptr) {}

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
                int operator<(DistancePoint<M> other) { return this->distance < other->distance; }
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

               for (DataPoint<T>* it = this->m_data.begin(); it != this->m_data.end(); it++, i++) {
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
     */
    template <typename T, typename M>
    void all_distances(std::string classified, std::string unclassified, int k,
            T (*converter)(std::string), misc::array<std::string> output_names,
            misc::array<M (*)(const DataPoint<misc::array<T>>&, const DataPoint<misc::array<T>>&)> distances);
}

