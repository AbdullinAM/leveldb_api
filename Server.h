//
// Created by abdullin on 2/26/16.
//

#ifndef LEVELDB_API_SERVER_H
#define LEVELDB_API_SERVER_H

#include <libsocket/headers/unixserverstream.hpp>
#include "Database.h"

namespace storage {

class Server {

public:

    const std::string DB_NAME = "/tmp/testbase";
    const std::string SOCKET_NAME = "/tmp/test_server_socket.soc";
    const int BUF_SIZE = 1024;

    Server();
    ~Server();

    int work();
    void destroy();

private:

    Database db_;
    libsocket::unix_stream_server server_;
    logging::Logger log_;
    char* buffer_;

};

}   /* namespace storage */

#endif //LEVELDB_API_SERVER_H
