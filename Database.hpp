//
// Created by abdullin on 2/18/16.
//

#ifndef LEVELDB_API_DATABASE_H
#define LEVELDB_API_DATABASE_H

#include <string>
#include <memory>
#include <iostream>

#include "leveldb/db.h"
#include "leveldb/comparator.h"

using namespace leveldb;

namespace storage {

class Database {

public:

    Database(const std::string& path) {
        Options options;
        options.create_if_missing = true;
        options.compression = kNoCompression;

        DB* cfg;
        Status status = DB::Open(options, path, &cfg);
        if (not status.ok()) {
            std::cout << "Error opening database " << status.ToString() << std::endl;
            exit(1);
        }
        db_.reset(cfg);
    }

private:

    std::shared_ptr<leveldb::DB> db_;

};

}   /* namespace storage */

#endif //LEVELDB_API_DATABASE_H
