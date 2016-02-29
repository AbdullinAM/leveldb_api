//
// Created by abdullin on 2/26/16.
//
# include "../headers/unixclientstream.hpp"
# include "../headers/exception.hpp"
# include <string>
# include <iostream>
# include <string.h>
#include <unistd.h>
#include <sstream>

int hexStringToInt(const char *str) {
    std::stringstream stream;
    stream << str;
    int num;
    stream >> std::hex >> num;
    return num;
}

int main() {
    using libsocket::unix_stream_client;
    using std::string;

    string path = "/tmp/test_server_socket.soc";
    char* answer = new char[128];

    memset(answer,0,128);

    try {
        while(true) {
            unix_stream_client sock(path);
            sock.snd("get", 3);
            sock.snd("00000003", 8);
            sock.snd("lol", 3);
            while (strcmp(answer, "end") != 0) {
                memset(answer, 0, 128);
                sock.rcv(answer, 8);
                int len = hexStringToInt(answer);
                memset(answer, 0, 128);
                sock.rcv(answer, len);
            }
            sock.snd("end", 3);
            sock.shutdown();
            sleep(1);
        }
    } catch (const libsocket::socket_exception& exc) {
        std::cerr << exc.mesg;
    }

    return 0;
}
