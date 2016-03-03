#ifndef DYNAMICAPP_DB_HPP
#define DYNAMICAPP_DB_HPP

#include <iostream>
#include <memory>

namespace leveldb_daemon {
namespace serializer {

struct Buffer{
    std::shared_ptr<char> array;
    size_t size;
};

template<class T, class ResT = char*>
struct serializer;

template<class ResT, class DataT, class Context>
struct deserializer;

template<class T, class ResT>
struct serializer {
    static Buffer serialize(const T &s) {
        return T::serialize(s);
    }
};

template<class ResT, class DataT, class Context>
struct deserializer {
    static auto deserialize(const DataT &s, Context &ctx) -> decltype(auto) {
        return ResT::deserialize(s, ctx);
    }

    static auto notFound() -> decltype(auto) {
        return ResT::notFound();
    }
};

}
}

#endif