//
// Created by abdullin on 2/26/16.
//
#include <iostream>
#include <serializer/Serializer.hpp>
#include <string.h>
#include "ipc/Client.h"
#include "DB.hpp"
#include "serStr.pb.h"


struct serializableStruct {
    int k;
    std::string l;

    serializableStruct() : k(0), l() {}
    serializableStruct(int k1, const std::string& l1) : k(k1), l(l1) {}

    static leveldb_daemon::serializer::Buffer serialize(const serializableStruct &){
        example::SerStr ser;
        ser.set_l("lol");
        ser.set_k(0);
        std::string serial;
        ser.SerializeToString(&serial);
        std::shared_ptr<char> sp(new char[10],std::default_delete<char[]>());
        for(int i=0;i<serial.size();++i){
            sp.get()[i]=serial[i];
        }
        return {sp, serial.size()};
    }

    static serializableStruct deserialize(const leveldb_daemon::serializer::Buffer& ser, const std::string& context) {
        example::SerStr des;
        std::string st=ser.array.get();
        des.ParseFromString(st);
        return serializableStruct((int)des.k(),des.l());
    }

    static serializableStruct notFound() {
        return serializableStruct();
    }

};


int main() {


    std::string ctx;
    example::SerStr ser;
    serializableStruct s = {0, "lol"};
    leveldb_daemon::db::write<serializableStruct>("key", s);
    auto&& res = leveldb_daemon::db::read<serializableStruct, std::string>("key", ctx);
    for (auto&& it: res) {
        std::cout << it.l << std::endl;
    }
    return 0;
}
