cmake_minimum_required(VERSION 3.3)

include_directories(./ ../)
include_directories(/usr/include/leveldb)
include_directories(libsocket/headers)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

project(leveldb_api)

set(SOURCE_FILES
        main.cpp
        Database.cpp
        Server.cpp
        Logger.h)

add_executable(leveldb_api ${SOURCE_FILES})

add_subdirectory(libsocket)
add_dependencies(leveldb_api socket++)

target_link_libraries(leveldb_api socket++)
target_link_libraries(leveldb_api leveldb)

project(client)

add_executable(client client.cpp)

add_dependencies(client socket++)

target_link_libraries(client socket++)