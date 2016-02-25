//
// Created by abdullin on 2/24/16.
//

#include <string.h>

#include "Server.h"

namespace ipc {

Server::Ptr Server::getInstance() {
    if (not instance_) {
        instance_ = Server::Ptr { new Server() };
    }
    return instance_;
}

Server::Server() : server_(SERVER_SOCKET) {
    buffer_ = new char[BUF_SIZE];
    memset(buffer_, 0, BUF_SIZE);
}

Server::~Server() {
    delete buffer_;
    server_.destroy();
}

Server::ArrayRef Server::recvFrom(std::string& client) {
    try {
        memset(buffer_, 0, BUF_SIZE);
        auto&& size = server_.rcvfrom(buffer_, BUF_SIZE, client);
        return ArrayRef(buffer_, (size_t)size);
    } catch (const libsocket::socket_exception& exc) {
        log_.print("Error: receive from failed");
        log_.print(exc.mesg);
        return leveldb::Slice();
    }
}

void Server::sendTo(const std::string &client, Server::ArrayRef message) {
    try {
        server_.sndto(message.data(), message.size(), client);
    } catch (const libsocket::socket_exception& exc) {
        log_.print("Error: send to failed");
        log_.print(exc.mesg);
    }
}

Server::Ptr Server::instance_ = nullptr;

}   /* namespace ipc */