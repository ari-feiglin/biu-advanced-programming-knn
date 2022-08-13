#include "serialization.h"

using namespace streams;

thread_local Serializer serializer = Serializer();

Serializer& operator<<(Serializer& s, const std::string str) {
    s << str.length() << streams::SerializablePointer(str.c_str(), str.length());
    return s;
}

Serializer& operator>>(Serializer& s, std::string& str) {
    size_t n;
    char* str;

    s >> n >> SerializablePointer(str, n).allocate();
    str = std::string(str, n);

    return s;
}

