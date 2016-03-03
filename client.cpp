//
// Created by abdullin on 2/26/16.
//
#include <iostream>
#include <serializer/Serializer.hpp>
#include <string.h>
#include "ipc/Client.h"
#include "DB.hpp"


struct serStr{
    int k;
    std::string l;
    serStr(int k1,std::string l1):k(k1),l(l1){}
    serStr():k(0),l(){}

    static leveldb_daemon::serializer::Buffer serialize(const serStr&){
        std::shared_ptr<char> l(new char);
        memcpy(l.get(),"toJSON",6);
        return {l,6};
    }

    static serStr deserialize(const std::string& ser,const std::string& context){
        auto&& s= serStr(3,ser);
        return s;
    }

    static serStr notFound(){
        return serStr();
    }

};


int main() {

    serStr s={0,"lol"};
    leveldb_daemon::db::write<serStr>("key",s);

    leveldb_daemon::ipc::Client client("/tmp/leveldb-test-server-socket.soc");

    auto&& res = client.get("key");
    for (auto&& it : res) {
        std::cout << it.first << std::endl;
    }
    return 0;
}
