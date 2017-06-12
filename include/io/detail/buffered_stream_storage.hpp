
// Adapted from asio/detail/buffered_stream_storage.hpp
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Modifications copyright (c) 2017 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MODERN_IO_DETAIL_BUFFERED_STREAM_STORAGE_HPP_INCLUDED
#define MODERN_IO_DETAIL_BUFFERED_STREAM_STORAGE_HPP_INCLUDED

#include <io/buffer.hpp>
#include <vector>

namespace io {
namespace detail {

template <typename Allocator = std::allocator<unsigned char>>
struct buffered_stream_storage {
    using byte_type = unsigned char;
    using size_type = std::size_t;
    using allocator_type = Allocator;

    explicit buffered_stream_storage(size_type max_size)
            : vec_(max_size)
    {}

    buffered_stream_storage(size_type max_size, const allocator_type& allocator)
            : vec_(max_size, allocator)
    {}

    void clear()
    {
        begin_ = 0;
        end_ = 0;
    }

    mutable_buffer data()
    {
        return io::buffer(vec_) + begin_;
    }

    const_buffer data() const
    {
        return io::buffer(vec_) + begin_;
    }

    bool empty() const
    {
        return begin_ == end_;
    }

    size_type size() const
    {
        return end_ - begin_;
    }

    void resize(size_type length)
    {
        assert(length <= capacity());
        if (begin_ + length <= capacity()) {
            end_ = begin_ + length;
        } else {
            std::move(vec_.begin() + begin_, vec_.begin() + size(), vec_.begin());
            end_ = length;
            begin_ = 0;
        }
    }

    size_type capacity() const
    {
        return vec_.size();
    }

    void consume(size_type count)
    {
        assert(begin_ + count <= end_);
        begin_ += count;
        if (empty()) {
            clear();
        }
    }

    byte_type front() const { return vec_[begin_]; }

private:
    size_type begin_ = 0;
    size_type end_ = 0;
    std::vector<byte_type, allocator_type> vec_;
};

}
}

#endif
