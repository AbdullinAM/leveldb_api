//
// Created by abdullin on 2/24/16.
//

#ifndef LEVELDB_API_SERVER_H
#define LEVELDB_API_SERVER_H

#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>

#include <slice.h>
#include <unixserverdgram.hpp>

#include "Logging/Logger.h"

namespace ipc {

class Server {

public:

    using Socket = int;
    using ArrayRef = leveldb::Slice;
    using Ptr = std::shared_ptr<Server>;

    static constexpr int BUF_SIZE = 64 * 1024;
    const std::string SERVER_SOCKET = "/tmp/leveldb_api_server.soc";

    static Server::Ptr getInstance();

    ~Server();

    ArrayRef recvFrom(std::string& client);
    void sendTo(const std::string& client, ArrayRef message);

private:

    Server();
    Server(const Server&) = delete;
    Server& operator=(Server&) = delete;

private:

    logging::Logger log_;
    libsocket::unix_dgram_server server_;
    char* buffer_;
    static Server::Ptr instance_;

};

}   /* namespace ipc */

#endif //LEVELDB_API_SERVER_H
