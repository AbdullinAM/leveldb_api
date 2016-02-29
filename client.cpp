//
// Created by abdullin on 2/26/16.
//
#include <iostream>
#include "ipc/Client.h"

int main() {
    using std::string;

    string path = "/tmp/leveldb-test-server-socket.soc";

    leveldb_daemon::ipc::Client client(path);

    char val1[] = "lol1";
    char val2[] = "lol2";
    client.put("datakey", val1, 4);
    client.put("datakey2", val2, 4);

    auto &&res = client.get("datakey");

    for (auto &&it: res) {
        std::cout << it.first << std::endl;
    }

    for (auto&& it: res) {
        delete it.first;
    }

    return 0;
}
