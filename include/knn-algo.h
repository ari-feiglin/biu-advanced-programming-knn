#pragma once

#include "knn.h"

namespace knn {
    namespace {
        template <typename T>
        int partition(std::vector<T>& vec, int l, int h, int pi) {
            T pivot = vec[pi];
            std::swap(vec[pi], vec[h]);
            int x = l;

            for (int i = l; i < h; i++) {
                if (vec[i] < pivot) {
                    std::swap(vec[x], vec[i]);
                    x++;
                }
            }

            std::swap(vec[h], vec[x]);

            return x;
        }

        template <typename T>
        int random_partition(std::vector<T>& vec, int l, int h) {
            int random_pivot = rand() % (h - l + 1) + l;
            return partition(vec, l, h, random_pivot);
        }
    } // anonymous

    /**
     * Creates a vector of the k-smallest elements in an input vector.
     * @param vec           The vector to investigate.
     * @param k             The k value used in the algorithm.
     * @note                The function uses the Quick Select algorithm and thus has an average time
     *                      complexity of O(n).
     *                      Furthermore, the elements (T) must be comparable with the < operator.
     */
    template <typename T>
    std::vector<T> quickselect(const std::vector<T>& vec, int k) {
        std::vector<T> new_vec = vec;
        int l = 0;
        int h = new_vec.size() - 1;
        int pi = 0;

        while (true) {
            if (l == h) return new_vec;
            pi = random_partition(new_vec, l, h);

            if (pi == k - 1) return new_vec;
            else if (k - 1 < pi) h = pi - 1;
            else l = pi + 1;
        }
    }
} // knn

