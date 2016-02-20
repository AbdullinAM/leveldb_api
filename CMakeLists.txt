cmake_minimum_required(VERSION 3.3)
project(leveldb_api)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

include_directories(/usr/include/leveldb)
include_directories(./ ../)

set(SOURCE_FILES main.cpp DB/Database.cpp
        SharedMemory/SharedMemoryObject.cpp
        System/Errors.h
        System/Permissions.h
        System/FileOperations.h)

add_executable(leveldb_api ${SOURCE_FILES})

target_link_libraries(leveldb_api libleveldb.so)    #link leveldb lib with target