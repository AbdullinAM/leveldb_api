cmake_minimum_required(VERSION 3.3)

include_directories(./ ../)
include_directories(/usr/include/leveldb)
include_directories(libsocket)
add_subdirectory(libsocket)
link_directories(libsocket/C++/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

project(leveldb_api)

set(SOURCE_FILES
        main.cpp
        DB/Database.cpp
        Server/Server.cpp)

add_executable(leveldb_api ${SOURCE_FILES})

target_link_libraries(leveldb_api libleveldb.so)    #link leveldb lib with target
target_link_libraries(leveldb_api libsocket++.so)

