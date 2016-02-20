//
// Created by abdullin on 2/20/16.
//

#ifndef LEVELDB_API_PERMISSIONS_HPP
#define LEVELDB_API_PERMISSIONS_HPP

namespace system_operations {

class Permissions {

public:

    using os_permissions_type = int;

    Permissions(os_permissions_type type)
            : m_perm(type) { }

    Permissions() { setDefault(); }

    void setDefault() {
        m_perm = 0644;
    }

    void setUnrestricted() {
        m_perm = 0666;
    }

    void setPermissions(os_permissions_type perm) { m_perm = perm; }

    os_permissions_type getPermissions() const { return m_perm; }

private:

    os_permissions_type m_perm;

};

}   /* namespace system_operations */

#endif //LEVELDB_API_PERMISSIONS_HPP
