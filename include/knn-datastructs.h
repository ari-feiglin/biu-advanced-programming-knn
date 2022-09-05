#pragma once

#include "knn.h"

namespace knn {
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

            virtual DataPoint<T>* clone() const =0;

            virtual bool operator==(const DataPoint<T>& other) const { return other.data() == this->data(); }
    };

    template <typename T>
    DataPoint<T>::~DataPoint() { }

    /**
     * Class representing a literal data **point**.
     * Stores data as an array.
     */
    template <typename T>
    class CartDataPoint : public DataPoint<misc::array<T>> {
        misc::array<T> m_data;
        std::string m_class_name;

        public:
            CartDataPoint() { }

            CartDataPoint(const CartDataPoint& other) :
                m_data(other.m_data), m_class_name(other.m_class_name) { }

            CartDataPoint(CartDataPoint&& other) {
                this->m_data = std::move(other.m_data);
                this->m_class_name = std::move(other.m_class_name);
                other.m_data = nullptr;
            }
            
            CartDataPoint(const misc::array<T> data) :
                m_data(data), m_class_name() {}

            CartDataPoint(std::string class_name, const misc::array<T> data) :
                m_data(data), m_class_name(class_name) {}

            ~CartDataPoint() { }

            CartDataPoint* clone() const override {
                return new CartDataPoint(*this);
            }
            
            std::string class_type() const override { return this->m_class_name; }

            const misc::array<T>& data() const override { return this->m_data; }

            template <typename M>
            friend streams::Serializer& operator<<(streams::Serializer& s, const CartDataPoint<M>& cdp);
            template <typename M>
            friend streams::Serializer& operator>>(streams::Serializer& s, CartDataPoint<M>& cdp);
    };

    template <typename T>
    streams::Serializer& operator<<(streams::Serializer& s, const CartDataPoint<T>& cdp) {
        return s << cdp.m_data << cdp.m_class_name;
    }

    template <typename T>
    streams::Serializer& operator>>(streams::Serializer& s, CartDataPoint<T>& cdp) {
        return s >> cdp.m_data >> cdp.m_class_name;
    }

    /**
     * Class for storing a collection of data points and manipulating them.
     */
    template <typename T>
    class DataSet {
        std::vector<DataPoint<T>*> m_data;

        public:
            /**
             * Default constructor for DataSet.
             */
            DataSet() {}

            DataSet(std::vector<DataPoint<T>*>& vec) : m_data(vec) { }

            DataSet(std::vector<DataPoint<T>*>&& vec) : m_data(vec) { }

            ~DataSet() {
                for (size_t i = 0; i < this->m_data.size(); i++) {
                    delete this->m_data[i];
                }
            }

            /**
             * Add a Data Point to the Data Set.
             * @param data_point        The point to add to the set.
             * @return                  A reference to this Data Set.
             */
            DataSet& add(const DataPoint<T>* data_point);

            std::string get_class(const DataPoint<T>* data_point) const {
                for (DataPoint<T>* dp : this->m_data) {
                    if (*dp == *data_point)  return dp->class_type();
                }
                return "";
            }
            
            /**
             * Gets the k-nearest neighbors to another input Data Point.
             * @param k             The k-value to run the algorithm on.
             * @param p             The point to find the nearest neighbors to.
             * @param distance      A function for computing distances between Data Points.
             * @return              An array whose first k elements are the closest neighbors to p.
             */
            template <typename M>
            DataPoint<T>** get_k_nearest(int k, const DataPoint<T>* p, M (*distance)(const DataPoint<T>*, const DataPoint<T>*)) const;

            /**
             * Gets the class name of the nearest class to a give input Data Point.
             * @param k             The k-value for the KNN algorithm.
             * @param p             The point to find neighbors relative to.
             * @param distance      A function for computing distances between Data Points.
             * @return              The name of the nearest class to p.
             */
            template <typename M>
            std::string get_nearest_class(int k, const DataPoint<T>* p, M (*distance)(const DataPoint<T>*, const DataPoint<T>*)) const;

            const std::vector<DataPoint<T>*>& get_data() { return this->m_data; }

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
                int operator<(DistancePoint<M> other) const { return this->distance < other.distance; }
            };

            /**
             * Transforms the vector of Data Points to a vector of Distance Points.
             * @param p             The point to find the distance relative to.
             * @param distance      The distance function.
             * @return              A vector of all the distances.
             */
            template <typename M>
            std::vector<DistancePoint<M>> transform_data(const DataPoint<T>* p, M (*distance)(const DataPoint<T>*, const DataPoint<T>*)) const {
               std::vector<DistancePoint<M>> distances;
               int i = 0;

               for (const DataPoint<T>* dp : this->m_data) {
                   distances.push_back(DistancePoint<M>(i, distance(p, dp)));
                   i++;
               }

               return distances;
            }
    };
}

#include "knn-datastructs.tpp"

