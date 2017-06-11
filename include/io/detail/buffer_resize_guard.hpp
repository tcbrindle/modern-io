
// Adapted from asio/detail/buffered_resize_guard.hpp
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Modifications copyright (c) 2017 Tristan Brindle (tcbrindle at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MODERN_IO_DETAIL_BUFFER_RESIZE_GUARD_HPP_INCLUDED
#define MODERN_IO_DETAIL_BUFFER_RESIZE_GUARD_HPP_INCLUDED

namespace io {
namespace detail {

template <typename Buffer>
struct buffer_resize_guard {

    buffer_resize_guard(Buffer& buffer)
        : buf_(buffer),
          old_size_(buffer.size())
    {}

    ~buffer_resize_guard()
    {
        if (old_size_ != std::numeric_limits<std::size_t>::max()) {
            buf_.resize(old_size_);
        }
    }

    void commit()
    {
        old_size_ = std::numeric_limits<std::size_t>::max();
    }

private:
    Buffer& buf_;
    std::size_t old_size_;
};

}
}

#endif
