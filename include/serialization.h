#pragma once

#include <vector>
#include <map>

#include "streams.h"

namespace streams {
    enum SerializationTokens    {send_token,
                                int_token,
                                string_token,
                                open_file_w_token,
                                open_file_r_token,
                                write_file_token,
                                read_file_token,
                                end_token};

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

            Serializer(const Serializer& other) { this->m_stream = other->m_stream; }

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

    /** Serialization for primitive types (int, char, etc. NOT pointers) **/

    template <typename T>
    Serializer& operator<<(Serializer& s, const T prim) {
        s.stream()->send(&prim, sizeof(prim));
        return s;
    }

    template <typename T>
    Serializer& operator>>(Serializer& s, T& prim) {
        prim = s.stream()->receive<T>();
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
        const T* copy_pointer;
        size_t copy_size;
        bool deserializable;

        SerializablePointer(T*& p, size_t& s) : pointer(p), size(s), deserializable(true) { }
        SerializablePointer(const T*&& p, size_t s) :
            pointer(SerializablePointer<T>::null_pointer), size(SerializablePointer<T>::no_size),
            copy_pointer(p), copy_size(s), deserializable(false) { }

        /**
         * Allocate the proper memory for the pointer.
         * @param check     If true, only allocate if pointer is null.
         * @return      A reference to this.
         * The purpose of this is so you can do something like:
         * s >> size >> SerializablePointer(pointer, size).allocate();
         * Which will deserialize and then allocate the proper space for pointer.
         */
        SerializablePointer<T>& allocate(bool check=false) {
            if (!this->deserializable) throw std::invalid_argument("this SerializablePointer cannot be allocated");
            if (!check || this->size == 0) this->pointer = nullptr;
            else if (!check || this->pointer == nullptr) {
                this->pointer = new T[this->size];
            }

            return *this;
        }

        private:
            static T* null_pointer;
            static size_t no_size;
    };

    template <typename T>
    Serializer& operator<<(Serializer& s, const SerializablePointer<T> sp) {
        if (sp.deserializable) s.stream()->send(sp.pointer, sp.size);
        else s.stream()->send(sp.copy_pointer, sp.copy_size);
        return s;
    }

    template <typename T>
    Serializer& operator>>(Serializer& s, SerializablePointer<T>& sp) {
        if (!sp.deserializable) throw std::invalid_argument("cannot deserialize this SerializablePointer");
        sp.pointer = s.stream()->receive(sp.size);
        return s;
    }

    /** Serialization for serializable object pointers **/
    /** The template types here refer to classes which have implemented the serialization and deserialization methods (<< and >>) **/

    template <typename T>
    Serializer& operator<<(Serializer& s, T* p) {
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

    inline Serializer& operator<<(Serializer& s, const std::string str) {
        s << str.length() << streams::SerializablePointer<char>(str.c_str(), str.length());
        return s;
    }

    inline Serializer& operator>>(Serializer& s, std::string& str) {
        size_t n;
        char* cstr;

        s >> n;
        s >> SerializablePointer<char>(cstr, n).allocate();
        str = std::string(cstr, n);
        if (n > 0) delete[] cstr;

        return s;
    }

    inline Serializer& operator<<(Serializer& s, const char* str) {
        return s << std::string(str);
    }

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

