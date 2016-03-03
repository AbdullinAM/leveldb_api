#include "ipc/Client.h"
#include "ipc/Server.h"
#include "serializer/Serializer.hpp"


namespace leveldb_daemon {
namespace db{

template<class T>
static bool write(const std::string& key, const T& obj) {
    auto byteStream = serializer::serializer<T>::serialize(obj);
    ipc::Client client(ipc::Server::DEFAULT_SOCKET_NAME);
    return client.put(key, byteStream.array.get(),byteStream.size);
};

template<class ResT, class Context>
static auto read(const std::string& key, Context& ctx) -> decltype(auto) {
    ipc::Client client(ipc::Server::DEFAULT_SOCKET_NAME);
    auto&& serializedData = client.get(key);
    std::vector<ResT> result;
    for (auto&& it: serializedData) {
        std::shared_ptr<char> ptr(it.first);
        serializer::Buffer value{ptr, it.second};
        result.push_back(serializer::deserializer<ResT, serializer::Buffer, Context>::deserialize(value, ctx));
    }
    return result;
};

}   /* namespace db */
}   /* namespace leveldb_daemon */