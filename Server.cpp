//
// Created by abdullin on 2/26/16.
//

#include "Server.h"

namespace storage {

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
    while(true) {
        std::string op, key;
        int key_size, data_size;

        try {
            libsocket::unix_stream_client* client;
            client = server_.accept();

            while (true) {
                op.resize(3);
                *client >> op;

                if (op == "end") {
                    delete client;
                    break;
                }

                int recv_key_size = client->rcv(buffer_, BUF_SIZE);
                key_size = atoi(buffer_);
                memset(buffer_, 0, recv_key_size);

                key.resize(key_size);
                *client >> key;

                if (op == "put") {
                    int recv_data_size = client->rcv(buffer_, BUF_SIZE);
                    data_size = atoi(buffer_);
                    memset(buffer_, 0, recv_data_size);

                    client->rcv(buffer_, data_size);
                    leveldb::Slice data(buffer_, data_size);

                    if (not db_.put(key, data)) {
                        log_.print("Error while putting data into db with key: " + key);
                    }
                    memset(buffer_, 0, data_size);

                } else if (op == "get") {
                    auto &&it = db_.get(key, key);
                    while (it.valid()) {
                        client->snd(it.value().data(), it.value().size());
                        it.next();
                    }
                    client->snd("end", 3);
                }
            }
        } catch (const libsocket::socket_exception& ex) {
            log_.print(ex.mesg);
        }
    }
}

}   /* namespace storage */