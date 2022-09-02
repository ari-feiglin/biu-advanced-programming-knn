#pragma once
#include <iostream>

#include "streams.h"
#include "serialization.h"

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
        size_t m_len;

        /**
         * Helper method for constructing an array.
         * Used for recursively expanding a parameter pack.
         */
        void cont_helper(int n) {
            this->m_len = n;
            if (n > 0) this->m_arr = new T[n];
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

            array() : m_len(0) { }
            
            /**
             * Copy constructor/assignment operator for array.
             */
            array(const array& arr) {
                this->m_len = arr.m_len;
                this->m_arr = new T[this->m_len];

                for (size_t i = 0; i < this->m_len; i++) {
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

            array(T* arr, size_t size) {
                this->m_len = size;
                this->m_arr = new T[size];

                for (int i = 0; i < size; i++) {
                    this->m_arr[i] = arr[i];
                }
            }

            array(size_t size) {
                this->m_len = size;
                this->m_arr = new T[size];
            }

            /**
             * Destructor.
             */
            ~array() {
                if (this->m_len > 0) {
                    delete[] this->m_arr;
                }
                this->m_len = 0;
            }

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
            size_t length() const { return this->m_len; }

            /**
             * Check if two arrays have the same length, throw an exception if they don't.
             * @param other     Another array.
             * @throws          std::invalid_argument if the arrays differ in length.
             */
            template <typename M>
            void assert_comparable(const array<M>& other) const {
                if (this->m_len != other.length()) {
                    throw std::invalid_argument("Arrays of incomparable lengths (" +
                            std::to_string(this->m_len) + " and " +
                            std::to_string(other.length()) + ")");
                }
            }

            bool operator==(const array<T>& other) const {
                if (this->m_len != other.m_len) return false;
                for (size_t i = 0; i < this->m_len; i++) {
                    if (this->m_arr[i] != other.m_arr[i]) return false;
                }
                return true;
            }

            template <typename M>
            friend streams::Serializer& operator<<(streams::Serializer& s, const array<M> arr);
            template <typename M>
            friend streams::Serializer& operator>>(streams::Serializer& s, array<M>& arr);
    };

    /** Serialization for arrays **/
    template <typename T>
    streams::Serializer& operator<<(streams::Serializer& s, const array<T> arr) {
        s << arr.m_len;
        for (int i = 0; i < arr.m_len; i++) {
            s << arr.m_arr[i];
        }

        return s;
    }

    template <typename T>
    streams::Serializer& operator>>(streams::Serializer& s, array<T>& arr) {
        if (arr.m_len > 0) { delete[] arr.m_arr; arr.m_len = 0; }
        s >> arr.m_len;
        arr.m_arr = new T[arr.m_len];

        for (int i = 0; i < arr.m_len; i++) {
            s >> arr.m_arr[i];
        }

        return s;
    }
}

