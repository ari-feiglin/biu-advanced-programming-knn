#pragma once

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
                helper(0, r...);
            }
            
            /**
             * Copy constructor/assignment operator for array.
             */
            array(const array& arr) {
                this->m_len = arr.m_len;
                this->m_arr = new T[this->m_len];

                for (int i = 0; i < this->m_arr; i++) {
                    this->m_arr[i] = arr.m_arr[i];
                }
            }

            array operator=(const array& arr) {
                delete[] this->m_arr;

                this->m_len = arr.m_len;
                this->m_arr = new T[this->m_len];

                for (int i = 0; i < this->m_arr; i++) {
                    this->m_arr[i] = arr.m_arr[i];
                }
            }

            /**
             * Move constructor/assignment operator for array.
             */
            array(array&& arr) {
                this->m_arr = arr.m_arr;
                this->m_len = arr.m_len;

                arr.m_arr = nullptr;
            }

            array operator=(array&& arr) {
                this->m_arr = arr.m_arr;
                this->m_len = arr.m_len;

                arr.m_arr = nullptr;
            }

            /**
             * Construct an array.
             * @param list      An rvalue reference to an initializer list allowing for
             *                  C-like definitions of an array.
             */
            array(std::initializer_list<T>&& list) {
                this->m_len = list.size();
                this->m_arr = new T[list.size()];

                int i = 0;
                for (auto it = list.start(); it != list.end(); it++, i++) {
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

            /**
             * @return The length of the array.
             */
            int length() { return this->m_len; }
    }
}

