#include "streams.h"

namespace streams {
    /**
     * Class for serializing objects.
     * In order to serialize objects o1,...,oN into a Stream stream and a Serializer serializer, do:
     * serializer(stream) << o1 << ... << oN;
     * And to deserialize:
     * serializer(stream) >> o1 >> ... >> oN;
     * If you'd like to deserialize a pointer p with n bytes, do:
     * serializer(stream) >> SerializablePointer(p, n);
     * Similar for serialization. So say you'd like to read the size of the pointer to deserialize and then deserialize, you can do:
     * serializer(stream) >> n >> SerializablePointer(p, n);
     */
    class Serializer {
        Stream* m_stream;

        public:
            /**
             * Construct a Serializer.
             */
            Serializer() : m_stream(nullptr) { }

            /**
             * Operator to set the current stream of the Serializer.
             * @param s     The stream.
             * @return      A reference to this.
             */
            Serializer& operator()(Stream* s) {
                this->m_stream = s;
                return *this;
            }

            /**
             * Gets the current stream in use.
             * @return      The current stream in use.
             */
            Stream* stream() {
                return this->m_stream;
            }
    };

    extern thread_local Serializer serializer;

    /** Serialization for primitive types (int, char, etc. NOT pointers) **/

    template <typename T>
    Serializer& operator<<(Serializer& s, const T prim) {
        s.stream()->send(&prim, sizeof(prim));
        return s;
    }

    template <typename T>
    Serializer& operator>>(Serializer& s, T& prim) {
        prim = s().stream()->receive<T>();
        return s;
    }

    /** Serialization for primitive pointer types **/
    
    /**
     * Struct for storing a pointer and its size.
     */
    template <typename T>
    struct SerializablePointer {
        T*& pointer;
        size_t& size;

        SerializablePointer(T*& p, size_t& s) : pointer(p), size(s) { }

        /**
         * Allocate the proper memory for the pointer.
         * @param check     If true, only allocate if pointer is null.
         * @return      A reference to this.
         * The purpose of this is so you can do something like:
         * s >> size >> SerializablePointer(pointer, size).allocate();
         * Which will deserialize and then allocate the proper space for pointer.
         */
        SerializablePointer<T>& allocate(bool check=false) {
            if (!check || this->pointer == nullptr) {
                this->pointer = new T[this->size];
            }

            return *this;
        }
    };

    template <typename T>
    Serializer& operator<<(Serializer& s, const SerializablePointer<T> sp) {
        s.stream()->send(sp.pointer, sp.size);
        return s;
    }

    template <typename T>
    Serializer& operator>>(Serializer& s, SerializablePointer<T>& sp) {
        s.stream()->receive(sp.pointer, sp.size);
        return s;
    }

    /** Serialization for serializable object pointers **/
    /** The template types here refer to classes which have implemented the serialization and deserialization methods (<< and >>) **/

    template <typename T>
    Serializer& operator<<(Serializer& s, const T* p) {
        return s << *p;
    }

    template <typename T>
    Serializer& operator>>(Serializer& s, T*& p) {
        /* If p is null, then allocate memory for it */
        if (p == nullptr) {
            p = new T();
        }

        return s >> *p;
    }

    Serializer& operator<<(Serializer& s, const std::string str);
    Serializer& operator>>(Serializer& s, std::string& str);

    template <typename T>
    Serializer& operator<<(Serializer& s, const std::vector<T> vec) {
        s << (size_t)vec.size();
        
        for (T val : vec) {
            s << vec;
        }

        return s;
    }

    template <typename T>
    Serializer& operator>>(Serializer& s, std::vector<T>& vec) {
        size_t size;
        s >> size;

        for (int i = 0; i < size; i++) {
            T val;
            s >> val;
            vec.push_back(val);
        }

        return s;
    }
}

