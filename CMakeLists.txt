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
        logging/Logger.h
        serializer/Serializer.hpp
        DB.hpp)

add_executable(leveldb_daemon ${SOURCE_FILES})

add_subdirectory(libsocket)
add_dependencies(leveldb_daemon socket++)

target_link_libraries(leveldb_daemon socket++)
target_link_libraries(leveldb_daemon leveldb)

project(client)

set(CLIENT_SOURCE_FILES
        client.cpp
        storage/Database.cpp
        ipc/Server.cpp
        ipc/Client.cpp
        logging/Logger.h
        serializer/Serializer.hpp
        DB.hpp)

add_executable(client ${CLIENT_SOURCE_FILES})

add_dependencies(client socket++)

target_link_libraries(client socket++)
target_link_libraries(client leveldb)