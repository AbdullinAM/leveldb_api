//
// Created by abdullin on 2/20/16.
//

//
// Created by abdullin on 2/20/16.
//

#include "SharedMemoryObject.h"

namespace shared_memory {

inline SharedMemoryObject::SharedMemoryObject() : m_handle(invalid_file()) {}

inline SharedMemoryObject::~SharedMemoryObject() { this->priv_close(); }


inline const char* SharedMemoryObject::get_name() const { return m_filename.c_str(); }

inline bool SharedMemoryObject::get_size(offset_type& size) const { return get_file_size(m_handle, size); }

inline void SharedMemoryObject::swap(SharedMemoryObject& other) {
    std::swap(m_handle,  other.m_handle);
    std::swap(m_mode,    other.m_mode);
    m_filename.swap(other.m_filename);
}

inline mapping_handle_type SharedMemoryObject::get_mapping_handle() const {
    return mapping_handle_from_file_handle(m_handle);
}

inline access_mode SharedMemoryObject::get_mode() const { return m_mode; }

inline bool SharedMemoryObject::priv_open_or_create(create_enum_type type, const char* filename, access_mode mode) {
    m_filename = filename;
    std::string shmfile;
    create_tmp_dir_and_get_filename(filename, shmfile);
    logging::Logger log;

    if (mode != read_write && mode != read_only) {
        log.print("Error in SharedMemoryObject::priv_open_or_create() wrong access mode");
        exit(1);
    }

    switch(type){
        case create_enum_type::DoOpen:
            m_handle = open_existing_file(shmfile.c_str(), mode, true);
            break;
        case create_enum_type::DoCreate:
            m_handle = create_new_file(shmfile.c_str(), mode, true);
            break;
        case create_enum_type::DoOpenOrCreate:
            m_handle = create_or_open_file(shmfile.c_str(), mode, true);
            break;
        default:
            log.print("Error in SharedMemoryObject::priv_open_or_create() wrong create type");
            exit(1);
    }

    //Check for error
    if(m_handle == invalid_file()){
        error_info err = system_error_code();
        this->priv_close();
        std::string error_messg;
        fill_system_message(err.get_error_code(), error_messg);
        log.print("Error in SharedMemoryObject::priv_open_or_create() wrong access mode");
        log.print("Error code: " + error_messg);
        exit(1);
    }

    m_mode = mode;
    return true;
}

inline bool SharedMemoryObject::remove(const char* filename) {
    try{
        //Make sure a temporary path is created for shared memory
        std::string shmfile;
        tmp_filename(filename, shmfile);
        return delete_file(shmfile.c_str()) == 0;
    }
    catch(...){
        return false;
    }
}

inline void SharedMemoryObject::truncate(offset_type length) {
    if(not truncate_file(m_handle, length)){
        error_info err = system_error_code();
        logging::Logger log;
        std::string error_messg;
        fill_system_message(err.get_error_code(), error_messg);
        log.print("Error in SharedMemoryObject::truncate()");
        log.print("Error code: " + error_messg);
        exit(1);
    }
}

inline void SharedMemoryObject::priv_close() {
    if(m_handle != invalid_file()){
        close_file(m_handle);
        m_handle = invalid_file();
    }
}

class removeSharedMemoryOnDestroy {
    const char * m_name;

public:

    removeSharedMemoryOnDestroy(const char* name) : m_name(name) {}
    ~removeSharedMemoryOnDestroy() { SharedMemoryObject::remove(m_name); }

};

}   /* namespace shared_memory */

