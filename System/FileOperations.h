//
// Created by abdullin on 2/20/16.
//

#ifndef LEVELDB_API_FILEFUNCTIONS_HPP
#define LEVELDB_API_FILEFUNCTIONS_HPP

#include <string>
#include <limits>
#include <climits>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <cstdio>
#include <dirent.h>
#include <sys/file.h>

#include <cstring>
#include <cstdlib>

#include "Errors.h"
#include "Permissions.h"
#include "Logging/Logger.h"

namespace system_operations {

using file_handle_type = int;
using offset_type = off_t;

struct mapping_handle_type {
    file_handle_type  handle;
    bool           is_xsi;
};

enum access_mode {
    read_only        = O_RDONLY
    , read_write     = O_RDWR
    , copy_on_write
    , read_private
    , invalid_mode   = 0xffff
};

enum file_pos_type {
    file_begin       = SEEK_SET
    , file_end       = SEEK_END
    , file_current   = SEEK_CUR
};

inline mapping_handle_type mapping_handle_from_file_handle(file_handle_type hnd);

inline file_handle_type file_handle_from_mapping_handle(mapping_handle_type hnd);

inline bool create_directory(const char* path);

inline const char *get_temporary_path();

inline file_handle_type create_new_file(const char* name,
                                        access_mode mode,
                                        const Permissions& perm = Permissions(),
                                        bool temporary = false);

inline file_handle_type create_or_open_file(const char* name,
                                            access_mode mode,
                                            const Permissions& perm = Permissions(),
                                            bool temporary = false);

inline file_handle_type open_existing_file(const char* name,
                                           access_mode mode,
                                           bool temporary = false);

inline mapping_handle_type mapping_handle_from_file_handle(file_handle_type hnd) {
    mapping_handle_type ret;
    ret.handle = hnd;
    ret.is_xsi = false;
    return ret;
}

inline file_handle_type file_handle_from_mapping_handle(mapping_handle_type hnd) { return hnd.handle; }

inline bool create_directory(const char* path) { return ::mkdir(path, 0777) == 0 && ::chmod(path, 0777) == 0; }

inline const char *get_temporary_path() {
    const char *names[] = {"/tmp", "TMPDIR", "TMP", "TEMP"};
    const int names_size = sizeof(names) / sizeof(names[0]);
    struct stat data;
    for (int i = 0; i != names_size; ++i) {
        if (::stat(names[i], &data) == 0) {
            return names[i];
        }
    }
    return "/tmp";
}

inline file_handle_type create_new_file(const char* name,
                                        access_mode mode,
                                        const Permissions& perm,
                                        bool temporary) {
    (void) temporary;
    int ret = ::open(name, ((int) mode) | O_EXCL | O_CREAT, perm.getPermissions());
    if (ret >= 0) {
        ::fchmod(ret, perm.getPermissions());
    }
    return ret;
}

inline file_handle_type create_or_open_file(const char* name,
                                            access_mode mode,
                                            const Permissions& perm,
                                            bool temporary) {
    (void) temporary;
    int ret = -1;
    //We need a loop to change permissions correctly using fchmod, since
    //with "O_CREAT only" ::open we don't know if we've created or opened the file.
    while (1) {
        ret = ::open(name, ((int) mode) | O_EXCL | O_CREAT, perm.getPermissions());
        if (ret >= 0) {
            ::fchmod(ret, perm.getPermissions());
            break;
        }
        else if (errno == EEXIST) {
            if ((ret = ::open(name, (int) mode)) >= 0 || errno != ENOENT) {
                break;
            }
        }
    }
    return ret;
}

inline file_handle_type open_existing_file(const char* name,
                                           access_mode mode,
                                           bool temporary) {
    (void) temporary;
    return ::open(name, (int) mode);
}

inline bool delete_file(const char *name) { return ::unlink(name) == 0; }

inline bool truncate_file(file_handle_type hnd, std::size_t size) {
    if (sizeof(off_t) == sizeof(std::size_t)) {
        if (size > ((~std::size_t(0)) >> 1)) {
            errno = EINVAL;
            return false;
        }
    }
    return 0 == ::ftruncate(hnd, off_t(size));
}

inline bool get_file_size(file_handle_type hnd, offset_type& size) {
    struct stat data;
    bool ret = 0 == ::fstat(hnd, &data);
    if (ret) {
        size = data.st_size;
    }
    return ret;
}

inline bool set_file_pointer(file_handle_type hnd, offset_type off, file_pos_type pos) {
    return ((off_t) (-1)) != ::lseek(hnd, off, (int) pos);
}

inline bool get_file_pointer(file_handle_type hnd, offset_type& off) {
    off = ::lseek(hnd, 0, SEEK_CUR);
    return off != ((off_t) -1);
}

inline bool write_file(file_handle_type hnd, const void* data, std::size_t numdata) {
    return (ssize_t(numdata)) == ::write(hnd, data, numdata);
}

inline file_handle_type invalid_file() { return -1; }

inline bool close_file(file_handle_type hnd) { return ::close(hnd) == 0; }

inline bool acquire_file_lock(file_handle_type hnd) {
    struct ::flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    return -1 != ::fcntl(hnd, F_SETLKW, &lock);
}

inline bool try_acquire_file_lock(file_handle_type hnd, bool& acquired) {
    struct ::flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    int ret = ::fcntl(hnd, F_SETLK, &lock);
    if (ret == -1) {
        return (errno == EAGAIN || errno == EACCES) ?
               acquired = false, true : false;
    }
    return (acquired = true);
}

inline bool release_file_lock(file_handle_type hnd) {
    struct ::flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    return -1 != ::fcntl(hnd, F_SETLK, &lock);
}

inline bool acquire_file_lock_sharable(file_handle_type hnd) {
    struct ::flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    return -1 != ::fcntl(hnd, F_SETLKW, &lock);
}

inline bool try_acquire_file_lock_sharable(file_handle_type hnd, bool& acquired) {
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    int ret = ::fcntl(hnd, F_SETLK, &lock);
    if (ret == -1) {
        return (errno == EAGAIN || errno == EACCES) ?
               acquired = false, true : false;
    }
    return (acquired = true);
}

inline bool release_file_lock_sharable(file_handle_type hnd) { return release_file_lock(hnd); }

inline bool delete_subdirectories_recursive
        (const std::string &refcstrRootDirectory, const char* dont_delete_this) {
    DIR *d = opendir(refcstrRootDirectory.c_str());
    if (!d) {
        return false;
    }

    struct dir_close {
        DIR *d_;

        dir_close(DIR *d) : d_(d) { }

        ~dir_close() { ::closedir(d_); }
    } dc(d);
    (void) dc;

    struct ::dirent *de;
    struct ::stat st;
    std::string fn;

    while ((de = ::readdir(d))) {
        if (de->d_name[0] == '.' && (de->d_name[1] == '\0'
                                     || (de->d_name[1] == '.' && de->d_name[2] == '\0'))) {
            continue;
        }
        if (dont_delete_this && std::strcmp(dont_delete_this, de->d_name) == 0) {
            continue;
        }
        fn = refcstrRootDirectory;
        fn += '/';
        fn += de->d_name;

        if (std::remove(fn.c_str())) {
            if (::stat(fn.c_str(), &st)) {
                return false;
            }
            if (S_ISDIR(st.st_mode)) {
                if (!delete_subdirectories_recursive(fn, 0)) {
                    return false;
                }
            } else {
                return false;
            }
        }
    }
    return std::remove(refcstrRootDirectory.c_str()) ? false : true;
}

inline void get_tmp_base_dir(std::string& tmp_name) {
    tmp_name = get_temporary_path();
    if(tmp_name.empty()){
        logging::Logger log;
        log.print("Error in get_tmp_base_dir: get_temporary_path() is empty");
        exit(1);
    }
    //Remove final null.
    tmp_name += "/interprocess";
}

inline void create_tmp_dir_and_get_filename(const char* filename, std::string& tmp_name) {
    //First get the temp directory
    get_tmp_base_dir(tmp_name);

    //If fails, check that it's because already exists
    if(not create_directory(tmp_name.c_str())){

        error_info info(system_error_code());
        if(info.get_error_code() != already_exists_error){
            logging::Logger log;
            std::string error_messg;
            fill_system_message(info.get_error_code(), error_messg);
            log.print("Error in create_tmp_dir_and_get_filename: create_directory() returned error");
            log.print("Error code: " + error_messg);
            exit(1);
        }
    }

    tmp_name += '/';
    tmp_name += filename;
}

inline void tmp_filename(const char* filename, std::string& tmp_name) {
    get_tmp_base_dir(tmp_name);
    //Remove final null.
    tmp_name += "/";
    tmp_name += filename;
}
}   /* namespace system_operations */

#endif //LEVELDB_API_FILEFUNCTIONS_HPP
