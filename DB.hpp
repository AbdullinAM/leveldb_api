#include "ipc/Client.h"
#include "ipc/Server.h"
#include "serializer/Serializer.hpp"


namespace leveldb_daemon {
namespace db{
template<class T>
static bool write(const std::string& key, const T& obj) {
    auto byteStream = serializer::serializer<T>::serialize(obj);
    ipc::Client client("/tmp/leveldb-test-server-socket.soc");
    client.put(key, byteStream.array.get(),byteStream.size);
    return true;
};

template<class ResT, class DataT, class Context>
static auto read(const std::string& key, Context& ctx) -> decltype(auto) {
    //get bytesStream from DB
    ipc::Client client("/tmp/leveldb-test-server-socket.soc");
    auto serObj=client.get(key);
    if (serObj.empty()) {
        return serializer::deserializer<ResT, DataT, Context>::notFound();
    }
    else {
        return serializer::deserializer<ResT, DataT, Context>::deserialize(serObj, ctx);
    }
};

}
}