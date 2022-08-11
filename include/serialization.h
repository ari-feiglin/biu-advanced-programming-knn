#pragma once

#include "streams.h"

/** MEMORY LEAKS FOR TO_SERIALIZABLE **/
/** Also fix <<= in misc **/

namespace streams {
    /**
     * Interface for Objects which can be serialized onto streams.
     */
    class Serializable {
        public:
            virtual ~Serializable() =0;

            /**
             * Serialize the object, sending it over a stream while doing so.
             * @param s     The stream to write to.
             */
            virtual void serialize(Stream* s) const =0;

            /**
             * Deserializes an object from a stream.
             * @param s     The stream to receive from.
             * @return      A new serialized object.
             * @note        This function is defined as const since deserializing should always create a new instance.
             *              The only exception to this is with PointerSerializable.
             */
            virtual Serializable* deserialize(Stream* s) const =0;

            template <typename T>
            static PrimitiveSerializable<T>* to_serializable(T p) { return new PrimitiveSerializable(p); }

            template <typename T>
            static PointerSerializable<T>* to_serializable(T* p, size_t size) { return new PointerSerializable(p, size); }

            static Serializable* to_serializable(Serializable& s) { return &s; }

            static PointerSerializable<T>* to_serializable(Serializable* s) { return new PointerSerializable(s); }

            /**
             * Assignment operator for serializables.
             * @param obj       An object to receive the serialized object.
             * @param s         The serialized object.
             * @return          A reference to the new object / obj.
             * @note            Normally this should be normal assignment.
             *                  But for PrimitiveSerializables this assigns the stored value.
             */
            template <typename T>
            friend T& operator<<=(T& obj, Serializable& s);
    };

    Serializable::~Serializable() { }

    template <typename T>
    T& operator<<=(T& obj, Serializable& s) {
        obj = s;
        return obj;
    }

    template <typename T>
    class PrimitiveSerializable : public Serializable {
        T m_prim;

        public:
            PrimitiveSerializable(T p) : m_prim(p) { }

            void serialize(Stream* s) override {
                s->send(&this->m_prim, sizeof(this->m_prim));
            }

            PrimitiveSerializable<T>* deserialize(Stream* s) override {
                this->m_prim = s->receive<T>();
                return this;
            }

            friend T& operator<<=(T& obj, PrimitiveSerializable<T>& s);
    };

    template <typename T>
    T& operator<<=(T& obj, PrimitiveSerializable<T>& s) {
        obj = s.m_prim;
        return obj;
    }

    /**
     * This class can store pointers, both pointers to primitives and pointers to Serializable objects.
     */
    template <typename T>
    class PointerSerializable : public Serializable {
        static size_t no_size = -1;
        T* m_pointer;
        size_t& m_size;

        public:
            /**
             * Construct Pointer Serializable object which stores the pointer of a Serializable object.
             * @param p     A pointer to a serializable object.
             */
            PointerSerializable(T* p) : m_pointer(p), m_size(no_size) { }

            /**
             * Construct a Pointer Serializable object which stores a pointer to a primitive.
             * @param p     A pointer to a primitive.
             */
            PointerSerializable(T* p, size_t& size) : m_pointer(p), m_size(size) { }

            void serialize(Stream* s) const override {
                if (this->m_size != -1) {
                    s->send(&this->m_size, sizeof(this->m_size));
                    s->send(this->m_pointer, this->m_size);
                } else {
                    this->m_pointer->serialize(s);
                }
            }

            PointerSerializable<T>* deserialize(Stream* s) const override {
                if (this->m_size != -1) {
                    this->m_size = s->receive<size_t>();
                    this->m_pointer = new T[this->m_size];
                    this->m_pointer = (T*)s->receive(this->m_size, true);
                } else {
                    this->m_pointer = this->m_pointer->deserialize(s);
                }

                return this;
            }

            friend T*& operator<<=(T*& obj, PointerSerializable<T>& ps);
    };

    template <typename T>
    T*& operator<<=(T*& obj, PointerSerializable<T>& ps) {
        obj = ps.m_pointer;
        return obj;
    }

    /**
     * Serialize a string.
     * @param str       The string to serialize.
     * @param s         The stream to serialize into.
     */
    inline void serialize(std::string str, Stream* s) {
        char* cstr = str.cstr();
        size_t size = str.length();
        PointerSerializable(cstr, size).serialize(s);
    }

    /**
     * Deserialize a string.
     * @param s     The stream to deserialize from.
     * @return      A string.
     */
    inline std::string deserialize(Stream* s) {
        char* cstr;
        size_t size;
        PointerSerializable(cstr, size).deserialize(s);
        return std::string(cstr, size);
    }
}

