//
// Created by abdullin on 2/19/16.
//

#ifndef LEVELDB_API_ARRAYREF_H
#define LEVELDB_API_ARRAYREF_H

#include <stddef.h>
#include <assert.h>
#include <bits/stl_iterator.h>

template <typename T>
class ArrayRef {

    using iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;

public:

    ArrayRef() : data_(nullptr), length_(0) {}

    ArrayRef(const T* data, size_t length) : data_(data), length_(length) {}

    ArrayRef(const T* begin, const T* end) : data_(begin), length_(end - begin) {}

    iterator begin() const { return data_; }
    iterator end() const { return data_ + length_; }

    reverse_iterator rbegin() const { return reverse_iterator(end()); }
    reverse_iterator rend() const { return reverse_iterator(begin()); }

    bool empty() const { return length_ == 0; }

    const T* data() const { return  data_; }
    size_t size() const { return length_; }

    const T& operator[] (size_t indx) {
        assert(indx < length_);
        return data_[indx];
    }

    const T& front() const {
        assert(not empty());
        return data_[0];
    }

    const T& back() const {
        assert(!empty());
        return data_[length_ - 1];
    }

private:

    const T* data_;
    size_t length_;

};


#endif //LEVELDB_API_ARRAYREF_H
