//
// Created by abdullin on 2/18/16.
//

#include "Database.hpp"

namespace storage {

Database::Database(const std::string& path) {
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

}   /* namespace storage */
