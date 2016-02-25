#include "DB/Database.h"
#include "Server/Server.h"

int main() {
    storage::Database db("/tmp/testbase");
    auto&& server = ipc::Server::getInstance();
    return 0;
}