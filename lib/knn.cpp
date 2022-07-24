#include "knn.h"

namespace knn {
    namespace {
        template <typename T>
        int partition(std::vector<T>& vec, int l, int h, int (*compare)(T, T)) {
            T pivot = vec[h];      // Choose the pivot to be the rightmost element
            while (l < h) {
                for (l--; compare(vec[l], pivot) > 0; l++);
                for (h++; compare(pivot, vec[h]) > 0; h--);
                if  (l >= h) return h;
                std::swap(vec[l], vec[h]);
            }
        }

        template <typename T>
        int random_partition(std::vector<T>& vec, int l, int h, int (*compare)(T, T)) {
            int random_pivot = rand() % (h - l + 1) + l;
            std::swap(vec[random_pivot], vec[h]);
            return partition(vec, l, h, compare);
        }
    } // anonymous

    template <typename T>
    std::vector<T> quickselect(const std::vector<T>& vec, int k, int (*compare)(T, T)) {
        std::vector<T> new_vec = vec;
        int l = 0;
        int h = new_vec.size() - 1;
        int pi = 0;

        while (true) {
            if (l == h) return new_vec;
            pi = random_partition(new_vec, l, h, compare);

            if (pi == k - 1) return new_vec;
            else if (k < pi) h = pi - 1;
            else l = pi + 1;
        }
    }
} // knn

