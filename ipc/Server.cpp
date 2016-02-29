//
// Created by abdullin on 2/26/16.
//

#include <sstream>
#include <unistd.h>

#include "Server.h"

namespace leveldb_daemon {
namespace ipc {

Server::Server() : db_(DB_NAME), server_(SOCKET_NAME), buf_size_(DEFAULT_BUF_SIZE) {
    buffer_ = new char[buf_size_];
    memset(buffer_, 0, buf_size_);
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

                std::string keySizeStr;
                keySizeStr.resize(WIDTH);
                *client >> keySizeStr;
                auto keySize = hexStringToInt(keySizeStr);

                key.resize(keySize);
                *client >> key;

                if (op == putCmd()) {
                    std::string dataSizeStr;
                    dataSizeStr.resize(WIDTH);
                    *client >> dataSizeStr;
                    auto dataSize = hexStringToInt(buffer_);
                    auto recvSize = client->rcv(buffer_, dataSize);
                    if (recvSize > DEFAULT_BUF_SIZE) resizeBuffer(recvSize);

                    leveldb::Slice data(buffer_, dataSize);

                    if (not db_.put(key, data)) {
                        log_.print("Error while putting data into db with key: " + key);
                        client->snd(failCmd().c_str(), CMD_LENGTH);
                    } else {
                        client->snd(succeedCmd().c_str(), CMD_LENGTH);
                    }
                    memset(buffer_, 0, dataSize);

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

std::string Server::intToHexString(const int num, const size_t width) {
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

int Server::hexStringToInt(const std::string& str) {
    std::stringstream stream;
    stream << str;
    int num;
    stream >> std::hex >> num;
    return num;
}

void Server::resizeBuffer(size_t size) {
    if (size > 0) {
        char* newBuffer = new char[size];
        memcpy(newBuffer, buffer_, buf_size_);
        delete buffer_;
        buffer_ = newBuffer;
        buf_size_ = size;
    } else {
        log_.print("Error: trying to initialize buffer with negative length");
    }
}

}   /* namespace ipc */
}   /* namespace leveldb_daemon */