//
// Created by abdullin on 2/26/16.
//

#ifndef LEVELDB_API_SERVER_H
#define LEVELDB_API_SERVER_H

#include <libsocket/headers/unixserverstream.hpp>

#include "Database.h"
#include "Logger.h"

namespace leveldb_daemon {
namespace ipc {

class Server {

public:

    const std::string DB_NAME = "/tmp/testbase";
    const std::string SOCKET_NAME = "/tmp/test_server_socket.soc";
    static const int BUF_SIZE = 64 * 1024;
    static const int WIDTH = 8;

    Server();

    ~Server();

    int work();

    void destroy();

private:

    std::string intToHexString(const int num, unsigned width = WIDTH);
    int hexStringToInt(const char *str);

    storage::Database db_;
    libsocket::unix_stream_server server_;
    logging::Logger log_;
    char *buffer_;

};

}   /* namespace ipc */
}   /* namespace leveldb_daemon */

#endif //LEVELDB_API_SERVER_H
