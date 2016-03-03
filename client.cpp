//
// Created by abdullin on 2/26/16.
//
#include <iostream>
#include <serializer/Serializer.hpp>
#include <string.h>
#include "ipc/Client.h"
#include "DB.hpp"


struct serializableStruct {
    int k;
    std::string l;

    serializableStruct() : k(0), l() {}
    serializableStruct(int k1, const std::string& l1) : k(k1), l(l1) {}

    static leveldb_daemon::serializer::Buffer serialize(const serializableStruct &){
        std::shared_ptr<char> l(new char);
        memcpy(l.get(), "toJSON", 6);
        return {l, 6};
    }

    static serializableStruct deserialize(const leveldb_daemon::serializer::Buffer& ser, const std::string& context) {
        auto&& s = serializableStruct(3, ser.array.get());
        return s;
    }

    static serializableStruct notFound() {
        return serializableStruct();
    }

};


int main() {
    serializableStruct s = {0, "lol"};
    leveldb_daemon::db::write<serializableStruct>("key", s);
    s.l = "asa";
    leveldb_daemon::db::write<serializableStruct>("key1", s);

    std::string ctx;
    auto&& res = leveldb_daemon::db::read<serializableStruct, std::string>("key", ctx);
    for (auto&& it: res) {
        std::cout << it.l << std::endl;
    }

    return 0;
}
