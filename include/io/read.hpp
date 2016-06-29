
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_READ_HPP
#define IO_READ_HPP

#include <io/buffer.hpp>

namespace io {

using net::read;
using net::read_until;

template <class SyncReadStream, class DynamicBuffer,
        class = std::enable_if_t<is_dynamic_buffer<std::decay_t<DynamicBuffer>>::value>>
std::size_t read_all(SyncReadStream& stream, DynamicBuffer&& b, std::error_code& ec)
{
    std::size_t bytes_read = io::read(stream, std::forward<DynamicBuffer>(b), ec);
    if (ec == stream_errc::eof) {
        ec.clear();
    }
    return bytes_read;
}

template <class SyncReadStream, class DynamicBuffer,
        class = std::enable_if_t<is_dynamic_buffer<std::decay_t<DynamicBuffer>>::value>>
std::size_t read_all(SyncReadStream& stream, DynamicBuffer&& b)
{
    std::error_code ec;
    std::size_t bytes_read = io::read_all(stream, std::forward<DynamicBuffer>(b), ec);
    if (ec) {
        throw std::system_error{ec};
    }
    return bytes_read;
}

}

#endif
