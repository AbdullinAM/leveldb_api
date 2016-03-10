//
// Created by abdullin on 2/26/16.
//
#include <iostream>
#include <serializer/Serializer.hpp>
#include "ipc/Client.h"
#include "DB.hpp"
#include "serStr.pb.h"


struct serializableStruct {
    int k;
    std::string l;

    serializableStruct() : k(0), l() {}
    serializableStruct(int k1, const std::string& l1) : k(k1), l(l1) {}

    static leveldb_daemon::serializer::Buffer serialize(const serializableStruct &s){
        example::SerStr ser;
        ser.set_l(s.l);
        ser.set_k(s.k);
        std::string serial;
        ser.SerializeToString(&serial);
        std::shared_ptr<char> sp(new char[10],std::default_delete<char[]>());
        for(int i=0;i<serial.size();++i){
            sp.get()[i]=serial[i];
        }
        return {sp, serial.size()};
    }

    static serializableStruct deserialize(const leveldb_daemon::serializer::Buffer& ser, std::string ctx="") {
        example::SerStr des;
        std::string st="";
        for(int i=0;i<ser.size;++i){
            st+=ser.array.get()[i];
        }
        des.ParseFromString(st);
        return serializableStruct((int)des.k(),des.l());
    }


    static serializableStruct notFound() {
        return serializableStruct();
    }

};


int main() {
    std::string ctx;
    serializableStruct s = {666, "SERSTRING"};
    leveldb_daemon::db::write<serializableStruct>("key", s);
    auto&& res = leveldb_daemon::db::read<serializableStruct>("key");
    for (auto&& it: res) {
        std::cout<<"Res="<<it.k<<" and "<<it.l<<"\n";
    }
    s = {777, "SERSTRING2"};
    leveldb_daemon::db::write<serializableStruct>("yek", s);
    res = leveldb_daemon::db::read<serializableStruct,std::string>("yek",ctx);
    for (auto&& it: res) {
        std::cout<<"Res2="<<it.k<<" and "<<it.l<<"\n";
    }
    return 0;
}
