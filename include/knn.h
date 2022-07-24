#pragma once

namespace knn {
    /**
     * Creates a vector of the k-smallest elements in an input vector.
     * @param vec           The vector to investigate.
     * @param k             The k value used in the algorithm.
     * @param compare       A function for comparing two T objects.
     *                      The compare function should return a positive value if the first element is
     *                      "less" than the second and otherwise return a negative value if the first
     *                      element is "more" than the second.
     *                      The return value in the case that the elements are equal must be non-positive.
     * @note                The function uses the Quick Select algorithm and thus has a time complexity
     *                      of O(n). Furthermore, the elements must be comparable with the < operator.
     */
    template <typename T>
    std::vector<T> quickselect(const std::vector<T>& vec, int k, int (*compare)(T, T);

    /**
     * Interface class for Data Points, 
     */
    template <typename T>
    class DataPoint {
        public:
            virtual ~DataPoint() =0;
            virtual std::string class_type() const =0;
            virtual T data() const =0;
    }

    template <typename T>
    class DataSet {
        std::vector<DataPoint<T>> m_data;

        public:
            DataSet(const std::vector<DataPoint<T>>& data);
            DataSet(std::vector<DataPoint<T>>&& data);

            DataSet(const DataSet& other);
            DataSet& operator=(const DataSet& other);

            DataSet(DataSet&& other);
            DataSet& operator=(DataSet&& other);

            DataSet& add(DataPoint<T> data_point);
            DataSet& remove(int index);

            DataPoint<T> * get_k_nearest(int k, int (*compare)(DataPoint<T>, DataPoint<T>));

            std::string get_nearest_class(int k, int (*compare)(DataPoint<T>, DataPoint<T>));
    }
}

