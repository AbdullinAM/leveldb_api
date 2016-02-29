//
// Created by abdullin on 2/26/16.
//

#ifndef LEVELDB_API_SERVER_H
#define LEVELDB_API_SERVER_H

#include <libsocket/headers/unixserverstream.hpp>

#include "storage/Database.h"
#include "logging/Logger.h"

namespace leveldb_daemon {
namespace ipc {

class Server {

public:

    const std::string DB_NAME = "/tmp/testbase";
    const std::string SOCKET_NAME = "/tmp/test_server_socket.soc";
    static const size_t DEFAULT_BUF_SIZE = 64 * 1024;
    static const size_t WIDTH = 8;
    static const size_t CMD_LENGTH = 3;

    Server();
    ~Server();

    int work();
    void destroy();

private:

    std::string putCmd() const      { return "put"; }
    std::string getCmd() const      { return "get"; }
    std::string endCmd() const      { return "end"; }
    std::string succeedCmd() const  { return "ok_"; }
    std::string failCmd() const     { return "nok"; }

    std::string intToHexString(const int num, const size_t width = WIDTH);
    int hexStringToInt(const char *str);
    void resizeBuffer(size_t size);

private:

    storage::Database db_;
    libsocket::unix_stream_server server_;
    logging::Logger log_;
    char* buffer_;
    size_t buf_size_;

};

}   /* namespace ipc */
}   /* namespace leveldb_daemon */

#endif //LEVELDB_API_SERVER_H
