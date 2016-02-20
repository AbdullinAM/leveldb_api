//
// Created by abdullin on 2/20/16.
//

#ifndef LEVELDB_API_ERRORS_HPP
#define LEVELDB_API_ERRORS_HPP

#include <stdarg.h>
#include <string>

#include <errno.h>
#include <cstring>

namespace system_operations {

using native_error_type = int;

enum error_code_type {
    no_error = 0,
    system_error,
    other_error,
    security_error,
    read_only_error,
    io_error,
    path_error,
    not_found_error,
    busy_error,
    already_exists_error,
    not_empty_error,
    is_directory_error,
    out_of_space_error,
    out_of_memory_error,
    out_of_resource_error,
    lock_error,
    sem_error,
    mode_error,
    size_error,
    corrupted_error,
    not_such_file_or_directory,
    invalid_argument,
    timeout_when_locking_error,
    timeout_when_waiting_error,
    owner_dead_error
};

struct ec_xlate {
    native_error_type sys_ec;
    error_code_type ec;
};

static const ec_xlate ec_table[] = {
        {EACCES,       security_error},
        {EROFS,        read_only_error},
        {EIO,          io_error},
        {ENAMETOOLONG, path_error},
        {ENOENT,       not_found_error},
        {EAGAIN,       busy_error},
        {EBUSY,        busy_error},
        {ETXTBSY,      busy_error},
        {EEXIST,       already_exists_error},
        {ENOTEMPTY,    not_empty_error},
        {EISDIR,       is_directory_error},
        {ENOSPC,       out_of_space_error},
        {ENOMEM,       out_of_memory_error},
        {EMFILE,       out_of_resource_error},
        {ENOENT,       not_such_file_or_directory},
        {EINVAL,       invalid_argument}
};

inline int system_error_code() {
    return errno;
}

inline void fill_system_message(int system_error, std::string& str) { str = std::strerror(system_error); }

inline error_code_type lookup_error(native_error_type err) {
    const ec_xlate *cur = &ec_table[0],
            *end = cur + sizeof(ec_table) / sizeof(ec_xlate);
    for (; cur != end; ++cur) {
        if (err == cur->sys_ec) return cur->ec;
    }
    return system_error; // general system error code
}

struct error_info {
    error_info(error_code_type ec = other_error) : m_nat(0), m_ec(ec) {}

    error_info(native_error_type sys_err_code)
            : m_nat(sys_err_code), m_ec(lookup_error(sys_err_code)) {}

    error_info &operator=(error_code_type ec) {
        m_nat = 0;
        m_ec = ec;
        return *this;
    }

    error_info &operator=(native_error_type sys_err_code) {
        m_nat = sys_err_code;
        m_ec = lookup_error(sys_err_code);
        return *this;
    }

    native_error_type get_native_error() const { return m_nat; }

    error_code_type get_error_code() const { return m_ec; }

private:

    native_error_type m_nat;
    error_code_type m_ec;

};

}   /* namespace system_operations */

#endif //LEVELDB_API_ERRORS_HPP
