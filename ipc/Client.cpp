//
// Created by abdullin on 2/29/16.
//

#include <sstream>
#include <string.h>

#include "Client.h"

namespace leveldb_daemon {
namespace ipc {

Client::Client(std::string &server) : client_(server), log_(LOG_FILE) { }

Client::~Client() {
    close();
}

Client::DataArray Client::get(const std::string& key) {
    DataArray result;

    try {
        client_ << getCmd();

        auto keySize = intToHexString(key.length());
        client_ << keySize << key;

        while(true) {
            std::string dataSize;
            dataSize.resize(WIDTH);
            client_ >> dataSize;

            auto size = hexStringToInt(dataSize);
            if (size < 0) {
                log_.print("Error: received data with negative length");
            }
            char* data = new char[size];
            client_.rcv(data, size);

            if (strcmp(data, endCmd().c_str()) == 0) {
                delete data;
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

        auto keySize = intToHexString(key.length());
        client_ << keySize << key;

        auto dataSize = intToHexString(size);
        client_ << dataSize;
        client_.snd(data, size);

        result.resize(3);
        client_ >> result;
    } catch (const libsocket::socket_exception& exc) {
        log_.print(exc.mesg);
    }

    return result == succeedCmd();
}

void Client::close() {
    client_ << endCmd();
    client_.shutdown();
    client_.destroy();
}

std::string Client::intToHexString(const int num, size_t width) {
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

int Client::hexStringToInt(const std::string& str) {
    std::stringstream stream;
    stream << str;
    int num;
    stream >> std::hex >> num;
    return num;
}

}   /* namespace ipc */
}   /* namespace leveldb_daemon */