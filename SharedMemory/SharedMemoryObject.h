//
// Created by abdullin on 2/20/16.
//

#ifndef LEVELDB_API_SHAREDMEMORYOBJECT_HPP
#define LEVELDB_API_SHAREDMEMORYOBJECT_HPP

#include "System/FileOperations.h"

namespace shared_memory {

enum create_enum_type {
    DoCreate,
    DoOpen,
    DoOpenOrCreate
};

struct create_only_type {};
struct open_only_type {};
struct open_or_create_type {};

using namespace system_operations;

class SharedMemoryObject {

public:

    SharedMemoryObject();

    SharedMemoryObject(create_only_type, const char* name, access_mode mode) {
        this->priv_open_or_create(create_enum_type::DoCreate, name, mode);
    }

    SharedMemoryObject(open_or_create_type, const char* name, access_mode mode) {
        this->priv_open_or_create(create_enum_type::DoOpenOrCreate, name, mode);
    }

    SharedMemoryObject(open_only_type, const char* name, access_mode mode) {
        this->priv_open_or_create(create_enum_type::DoOpen, name, mode);
    }

    ~SharedMemoryObject();

    void swap(SharedMemoryObject& moved);
    static bool remove(const char *name);
    void truncate(offset_type length);

    const char* get_name() const;
    bool get_size(offset_type& size) const;
    access_mode get_mode() const;
    mapping_handle_type get_mapping_handle() const;

private:

    void priv_close();
    bool priv_open_or_create(create_enum_type type, const char *filename, access_mode mode);

private:

    file_handle_type  m_handle;
    access_mode         m_mode;
    std::string     m_filename;

};

}   /* namespace shared_memory */

#endif //LEVELDB_API_SHAREDMEMORYOBJECT_HPP
