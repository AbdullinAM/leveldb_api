//
// Created by abdullin on 2/26/16.
//
# include <iostream>
#include <ipc/Client.h>

int main() {
    using std::string;

    string path = "/tmp/test_server_socket.soc";

    while(true) {
        leveldb_daemon::ipc::Client client(path);

        client.put("datakey", "lol1", 4);
        client.put("datakey2", "lol2", 4);

        auto &&res = client.get("datakey");

        for (auto&& it: res) {
            delete it.first;
        }

        std::cout << "finish" << std::endl;
    }

    return 0;
}
