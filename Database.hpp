//
// Created by abdullin on 2/18/16.
//

#ifndef LEVELDB_API_DATABASE_H
#define LEVELDB_API_DATABASE_H

#include <string>
#include <memory>
#include <iostream>

#include "leveldb/db.h"

namespace storage {

using namespace leveldb;

class Database {

public:

    Database(const std::string& path);

    template <class Key, class Value>
    bool put(Key key, Value value) {

        const auto status = db_->Put(WriteOptions(),
                                   Slice(reinterpret_cast<const char*>(&key), sizeof(key)),
                                   Slice(reinterpret_cast<char*>(&value), sizeof(value)));

        if (not status.ok()) {
            std::cout << "Error put " << status.ToString() << std::endl;
        }

        return status.ok();
    };

    template <class Key, class Value>
    Value get(Key key) {

        auto&& iter(db_->NewIterator(leveldb::ReadOptions()));

        iter->Seek(Slice(reinterpret_cast<const char*>(&key), sizeof(key)));

        return *reinterpret_cast<const Value*>(iter->value().data());
    };

private:

    std::shared_ptr<leveldb::DB> db_;

};



}   /* namespace storage */

#endif //LEVELDB_API_DATABASE_H
