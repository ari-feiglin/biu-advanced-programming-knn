#pragma once

#include "streams.h"

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
            template <typename T = Serializable*>
            virtual Serializable* deserialize(Stream* s) const =0;

            template <typename T>
            static PrimitiveSerializable<T> get_serializable(T p) { return PrimitiveSerializable(p); }

            template <typename T>
            static PointerSerializable<T> get_serializable(T* p, size_t size) { return PointerSerializable(p, size); }

            static Serializable get_serializable(Serializable& s) { return s; }

            static Serializable get_serializable(Serializable* s) { return *s; }

            /**
             * Assignment operator for serializables.
             * @param obj       An object to receive the serialized object.
             * @param s         The serialized object.
             * @return          A reference to the new object / obj.
             * @note            Normally this should be normal assignment.
             * But for PrimitiveSerializables this assigns the stored value.
             */
            template <typename T>
            friend T& operator<<=(T& obj, Serializable& s);
    };

    Serializable::~Serializable() { }

    template <typename T>
    T& operator<<=(T& obj, Serializable& s) {
        obj = s;
    }

    template <typename T>
    class PrimitiveSerializable : public Serializable {
        T m_prim;

        public:
            PrimitiveSerializable(T p) : m_prim(p) { }

            void serialize(Stream* s) override {
                s->send(&this->m_prim, sizeof(this->m_prim));
            }

            Serializable deserialize(Stream* s) override {
                this->m_prim = (T)s->receive(sizeof(T), true);
                return *this;
            }

            template <typename T>
            friend T& operator<<=(T& obj, PrimitiveSerializable<T>& s);
    };

    template <typename T>
    T& operator<<=(T& obj, PrimitiveSerializable<T>& s) {
        obj = s.m_prim;
        return obj;
    }

    template <typename T>
    class PointerSerializable : public Serializable {
        T*& m_pointer;
        size_t& m_size;

        public:
            PointerSerializable(T*& p, size_t& size) : m_pointer(p), m_size(size) { }

            void serialize(Stream* s) const override {
                s->send(&this->m_size, sizeof(this->m_size));
                s->send(this->m_pointer, this->m_size);
            }

            Serializable deserialize(Stream* s) const override {
                this->m_size = (size_t)s->receive(sizeof(this->m_size), true);
                this->m_pointer = new T[this->m_size];
                this->m_pointer = (T*)s->receive(this->m_size, true);
                return nullptr;
            }
    };

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

