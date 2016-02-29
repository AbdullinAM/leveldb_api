//
// Created by abdullin on 2/26/16.
//

#include <sstream>
#include <unistd.h>

#include "Server.h"

namespace leveldb_daemon {
namespace ipc {

Server::Server() : db_(DB_NAME), server_(SOCKET_NAME) {
    buffer_ = new char[BUF_SIZE];
    memset(buffer_, 0, BUF_SIZE);
}

Server::~Server() {
    destroy();
}

void Server::destroy() {
    server_.destroy();
    delete buffer_;
}

int Server::work() {
    while (true) {
        std::string op, key;
        int key_size, data_size;

        try {
            libsocket::unix_stream_client* client;
            client = server_.accept();

            while (true) {
                op.resize(CMD_LENGTH);
                *client >> op;

                if (op == endCmd()) {
                    client->shutdown();
                    delete client;
                    break;
                }

                auto recv_key_size = client->rcv(buffer_, WIDTH);
                key_size = hexStringToInt(buffer_);
                memset(buffer_, 0, recv_key_size);

                key.resize(key_size);
                *client >> key;

                if (op == putCmd()) {
                    auto recv_data_size = client->rcv(buffer_, WIDTH);
                    data_size = hexStringToInt(buffer_);
                    memset(buffer_, 0, recv_data_size);

                    client->rcv(buffer_, data_size);
                    leveldb::Slice data(buffer_, data_size);

                    if (not db_.put(key, data)) {
                        log_.print("Error while putting data into db with key: " + key);
                        client->snd(failCmd().c_str(), CMD_LENGTH);
                    } else {
                        client->snd(succeedCmd().c_str(), CMD_LENGTH);
                    }
                    memset(buffer_, 0, data_size);

                } else if (op == getCmd()) {
                    auto&& it = db_.get(key);
                    while (it.valid()) {
                        auto&& size = intToHexString(it.value().size());
                        client->snd(size.c_str(), size.length());
                        client->snd(it.value().data(), it.value().size());
                        it.next();
                    }
                    auto&& size = intToHexString(CMD_LENGTH);
                    client->snd(size.c_str(), size.length());
                    client->snd(endCmd().c_str(), CMD_LENGTH);
                }
            }
        } catch (const libsocket::socket_exception& ex) {
            log_.print(ex.mesg);
        }
    }
}

std::string Server::intToHexString(const int num, unsigned int width) {
    std::string res;
    std::stringstream stream;
    stream << std::hex << num;
    stream >> res;
    if (res.length() < width) {
        std::string nulls(width - res.length(), '0');
        res.insert(0, nulls);
    } else if (res.length() > width) {
        log_.print("Error: size of data is too big");
        res = std::string(width, '0');
    }
    return res;
}

int Server::hexStringToInt(const char* str) {
    std::stringstream stream;
    stream << str;
    int num;
    stream >> std::hex >> num;
    return num;
}

}   /* namespace ipc */
}   /* namespace leveldb_daemon */