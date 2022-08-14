#include "serialization.h"

namespace streams {
    //thread_local Serializer serializer = Serializer();
    
    /*
    template <typename T>
    T* SerializablePointer<T>::null_pointer = nullptr;
    template <typename T>
    size_t SerializablePointer<T>::no_size = 0;
    */
    /*
    Serializer& operator<<(Serializer& s, const std::string str) {
        s << str.length() << streams::SerializablePointer<char>(str.c_str(), str.length());
        return s;
    }
    
    Serializer& operator>>(Serializer& s, std::string& str) {
        size_t n;
        char* cstr;
    
        s >> n >> SerializablePointer<char>(cstr, n).allocate();
        str = std::string(str, n);
        delete[] cstr;
    
        return s;
    }
    */

}

