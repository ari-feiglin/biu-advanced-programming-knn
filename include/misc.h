#pragma once
#include <iostream>

namespace misc {
    
    /**
     * array class for creating static-length arrays.
     * This array class differs from std::array in that it implicitly determines the length
     * of the array at initialization/assignment instead of having to be explicitly spoonfed
     * it.
     */
    template <typename T>
    class array {
        T * m_arr;
        int m_len;

        /**
         * Helper method for constructing an array.
         * Used for recursively expanding a parameter pack.
         */
        void cont_helper(int n) {
            this->m_len = n;
            this->m_arr = new T[n];
        }

        template <typename... R>
        void cont_helper(int n, T t, R... r) {
            cont_helper(n + 1, r...);
            this->m_arr[n] = t;
        }

        public:
            /**
             * Constructs an array.
             * @param r...      A list of parameters to initialize the array with.
             */
            template <typename... R>
            array(R... r) : m_len(0) {
                cont_helper(0, r...);
            }
            
            /**
             * Copy constructor/assignment operator for array.
             */
            array(const array& arr) {
                this->m_len = arr.m_len;
                this->m_arr = new T[this->m_len];

                for (int i = 0; i < this->m_len; i++) {
                    this->m_arr[i] = arr.m_arr[i];
                }
            }

            array& operator=(const array& arr) {
                if (this == &arr) return *this;

                delete[] this->m_arr;

                this->m_len = arr.m_len;
                this->m_arr = new T[this->m_len];

                for (int i = 0; i < this->m_arr; i++) {
                    this->m_arr[i] = arr.m_arr[i];
                }

                return *this;
            }

            /**
             * Move constructor/assignment operator for array.
             */
            array(array&& arr) {
                this->m_arr = arr.m_arr;
                this->m_len = arr.m_len;

                arr.m_arr = nullptr;
            }

            array& operator=(array&& arr) {
                if (this == &arr) return *this;

                delete[] this->m_arr;

                this->m_arr = arr.m_arr;
                this->m_len = arr.m_len;

                arr.m_arr = nullptr;

                return *this;
            }

            /**
             * Construct an array.
             * @param list      An rvalue reference to an initializer list allowing for
             *                  C-like definitions of an array.
             */
            array(std::initializer_list<T>&& list) {
                this->m_len = list.size();
                this->m_arr = new T[this->m_len];

                int i = 0;
                for (auto it = list.begin(); it != list.end(); it++, i++) {
                    this->m_arr[i] = *it;
                }
            }

            /**
             * Construct an array from vector.
             * @param vec       The vector to construct from.
             */
            array(std::vector<T>& vec) {
                this->m_len = vec.size();
                this->m_arr = new T[this->m_len];

                int i = 0;
                for (auto it = vec.begin(); it != vec.end(); it++, i++) {
                    this->m_arr[i] = *it;
                }
            }

            /**
             * Destructor.
             */
            virtual ~array() { delete[] this->m_arr; }

            /**
             * Indexing operator.
             * @param i     The index whose value to return.
             * @return      The value at index i.
             */
            T& operator[](int i) { return this->m_arr[i]; }
            T operator[](int i) const { return this->m_arr[i]; }

            /**
             * @return The length of the array.
             */
            int length() const { return this->m_len; }

            /**
             * Check if two arrays have the same length, throw an exception if they don't.
             * @param other     Another array.
             * @throws          std::invalid_argument if the arrays differ in length.
             */
            template <typename M>
            void assert_comparable(const array<M>& other) {
                if (this->m_len != other.length()) {
                    throw std::invalid_argument("Arrays of incomparable lengths (" +
                            std::to_string(this->m_len) + " and " +
                            std::to_string(other.length()) + ")");
                }
            }
    };
}

