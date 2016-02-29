//
// Created by abdullin on 2/26/16.
//
# include <iostream>
#include <ipc/Client.h>

int main() {
    using std::string;

    string path = "/tmp/test_server_socket.soc";

    leveldb_daemon::ipc::Client client(path);

    client.put("datakey", "lol1", 4);
    client.put("datakey2", "lol2", 4);

    auto&& res = client.get("datakey");

    if (res.empty()) std::cout << "empty" << std::endl;

    for (auto&& it : res) {
        std::cout << it.first << std::endl;
    }

    return 0;
}
