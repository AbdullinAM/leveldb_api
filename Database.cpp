//
// Created by abdullin on 2/18/16.
//

#include "Database.h"

namespace storage {

Database::Database(const std::string &path) {
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

bool Database::put(std::string& key, Value value) {
    const auto status = db_->Put(WriteOptions(), Slice(key), Slice(value.data(), value.size()));

    if (not status.ok()) {
        std::cout << "Error while putting:" << status.ToString() << std::endl;
    }

    return status.ok();
}

Database::Value Database::get(Database::Key &key) {
    auto&& it = db_->NewIterator(ReadOptions());

    it->Seek(Slice(key));
    return it->value();
}

Database::Iterator Database::get(Key& from, Key& to) {
    auto&& it = db_->NewIterator(ReadOptions());

    it->Seek(Slice(from));
    return Database::Iterator(it, to);
}

Database::Iterator::Iterator() : it_(nullptr) {}

Database::Iterator::Iterator(leveldb::Iterator* it, const Key& limit) : it_(it), limit_(limit) {}

void Database::Iterator::next() {
    if (it_ && it_->Valid()) {
        it_->Next();
    }
}

bool Database::Iterator::valid() const {
    if (it_) return false;

    const Slice limit(limit_);

    return it_->Valid() && it_->key().starts_with(limit);
}

Database::Value Database::Iterator::value() const {
    if (not it_) return Value();
    else return it_->value();
}

}   /* namespace storage */
