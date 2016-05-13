//
// Created by abdullin on 2/29/16.
//

#include <string.h>

#include "Client.h"
#include "util/Util.h"

namespace leveldb_daemon {
namespace ipc {

Client::Client() : log_(LOG_FILE), opened_(false) { }
Client::Client(const std::string &server) : client_(server), log_(LOG_FILE), opened_(true) { }

Client::~Client() {
    close();
}

void Client::connect(const std::string& server) {
    client_.connect(server);
    opened_ = true;
}

std::pair<char*, size_t> Client::get(const std::string& key) {
    try {
        client_ << getOneCmd();
        log_.print("Sending: " + getOneCmd());

        auto keySize = util::intToHexString(key.length(), WIDTH);
        client_ << keySize << key;
        log_.print("Sending key: " + key);

        std::string dataSize;
        dataSize.resize(WIDTH);
        client_ >> dataSize;

        auto size = util::hexStringToInt(dataSize);
        if (size < 0) {
            log_.print("Error: received data with negative length");
        } else if (size == 0) {
            log_.print("Empty input");
            return {nullptr, 0};
        }

        auto data = new char[size];
        memset(data, 0, size);
        receiveData(data, size);

        return {data, size};
    } catch (const libsocket::socket_exception& exc) {
        log_.print(exc.mesg);
    }
}

Client::DataArray Client::getAll(const std::string &key) {
    DataArray result;

    try {
        client_ << getAllCmd();
        log_.print("Sending: " + getAllCmd());

        auto keySize = util::intToHexString(key.length(), WIDTH);
        client_ << keySize << key;
        log_.print("Sending key: " + key);

        while(true) {
            std::string dataSize;
            dataSize.resize(WIDTH);
            client_ >> dataSize;
            auto size = util::hexStringToInt(dataSize);
            if (size < 0) {
                log_.print("Error: received data with negative length");
            } else if (size == 0) {
                log_.print("Empty input");
                continue;
            }

            auto data = new char[size];
            memset(data, 0, size);

            receiveData(data, size);

            if (size == CMD_LENGTH && std::string(data, CMD_LENGTH) == endCmd()) {
                log_.print("Received: " + endCmd());
                delete []data;
                break;
            }

            result.push_back({data, size});
        }
    } catch (const libsocket::socket_exception& exc) {
        log_.print(exc.mesg);
    }

    return result;
}

bool Client::put(const std::string& key, char* data, size_t size) {
    std::string result;

    try {
        client_ << putCmd();
        log_.print("Sending: " + putCmd());

        auto keySize = util::intToHexString(key.length(), WIDTH);
        client_ << keySize << key;
        log_.print("Sending key: " + key);

        auto dataSize = util::intToHexString(size, WIDTH);
        client_ << dataSize;
        log_.print("Sending data size: " + dataSize);
        if (size > 0) client_.snd(data, size);

        result.resize(3);
        client_ >> result;
    } catch (const libsocket::socket_exception& exc) {
        log_.print(exc.mesg);
    }

    return result == successCmd();
}

void Client::close() {
    if (opened_) {
        client_ << endCmd();
        client_.shutdown();
        client_.destroy();

        log_.print("Client destroyed");

        opened_ = false;
    }
}

bool Client::receiveData(char *buffer, size_t size) {
    auto totalRecvd = 0;
    log_.print("Receiving data with size:");
    log_.print(size);
    while (totalRecvd < size) {
        auto recvd = client_.rcv(buffer + totalRecvd, size - totalRecvd);
        if (recvd < 0) {
            log_.print("Error while receiving");
            return false;
        }
        totalRecvd += recvd;
    }
    log_.print("Receiving end");
    return true;
}

}   /* namespace ipc */
}   /* namespace leveldb_daemon */