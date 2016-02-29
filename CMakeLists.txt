cmake_minimum_required(VERSION 3.3)

include_directories(./ ../)
include_directories(/usr/include/leveldb)
include_directories(libsocket/headers)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

project(leveldb_daemon)

set(SOURCE_FILES
        main.cpp
        storage/Database.cpp
        ipc/Server.cpp
        ipc/Client.cpp
        logging/Logger.h)

add_executable(leveldb_daemon ${SOURCE_FILES})

add_subdirectory(libsocket)
add_dependencies(leveldb_daemon socket++)

target_link_libraries(leveldb_daemon socket++)
target_link_libraries(leveldb_daemon leveldb)

project(client)

add_executable(client client.cpp ipc/Client.cpp)

add_dependencies(client socket++)

target_link_libraries(client socket++)