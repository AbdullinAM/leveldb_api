//
// Created by abdullin on 2/18/16.
//

#ifndef LEVELDB_API_DATABASE_H
#define LEVELDB_API_DATABASE_H

#include <memory>

#include "leveldb/db.h"
#include "leveldb/comparator.h"

#include "Logger.h"

using namespace leveldb;

namespace storage {

class Database {

public:

    using Key = std::string;
    using Value = Slice;

    class Iterator;

    Database(const std::string& path);

    bool put(Key& key, Value value);
    Value get(Key& key);
    Iterator get(Key& from, Key& to);

private:

    std::shared_ptr<leveldb::DB> db_;
    logging::Logger logger_;

};

class Database::Iterator {

    using dbIterator = std::shared_ptr<leveldb::Iterator>;

public:

    Iterator();
    Iterator(leveldb::Iterator* it, const Key& limit);

    void next();
    bool valid() const;
    Value value() const;

private:

    dbIterator it_;
    Key limit_;

};

}   /* namespace storage */

#endif //LEVELDB_API_DATABASE_H
