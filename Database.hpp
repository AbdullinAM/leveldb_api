//
// Created by abdullin on 2/18/16.
//

#ifndef LEVELDB_API_DATABASE_H
#define LEVELDB_API_DATABASE_H

#include <string>
#include <memory>
#include <iostream>

#include "leveldb/db.h"

using namespace leveldb;

namespace storage {

class Database {

public:

    template <class Key, class Value>
    class Iterator;

    Database(const std::string& path);
    ~Database()    = default;

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
    Iterator<Key, Value> get(Key key, Key limit) {

        auto&& iter(db_->NewIterator(leveldb::ReadOptions()));

        iter->Seek(Slice(reinterpret_cast<const char*>(&key), sizeof(key)));

        return Iterator<Key, Value>(iter, limit);
    };

private:

    std::shared_ptr<leveldb::DB> db_;

};

template <class Key, class Value>
class Database::Iterator {

    using dbIterator = std::shared_ptr<leveldb::Iterator>;

public:

    Iterator()  = default;
    Iterator(const dbIterator& it, const Key& limit) : it_(it), limit_(limit) {}

    bool valid() {
        if (not it_) return false;

        const Slice limit{reinterpret_cast<const char*>(limit_), sizeof(limit_)};

        auto&& starts_with = [](std::string& text, std::string& token) -> bool {
            if (text.length() < token.length()) return false;

            return (text.compare(0, token.length(), token) == 0);
        };

        return it_->Valid() && starts_with(it_->key().ToString(), limit.ToString());
    }

    void next() {
        if (it_ && it_->Valid()) {
            it_->Next();
        }
    }

    Value value() {
        if (not it_) return Value();

        return *reinterpret_cast<const Value*>(it_->value().data());
    }

private:

    dbIterator it_;
    Key limit_;

};

}   /* namespace storage */

#endif //LEVELDB_API_DATABASE_H
