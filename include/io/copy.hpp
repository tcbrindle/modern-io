
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MODERN_IO_COPY_HPP
#define MODERN_IO_COPY_HPP

#include <io/io_concepts.hpp>
#include <io/read.hpp>

#include <cassert>

namespace io {

/// Copies the whole of one stream into another
template <typename ReadStream, typename WriteStream,
          typename = std::enable_if_t<is_sync_read_stream_v<ReadStream> &&
                                      is_sync_write_stream_v<WriteStream>>>
std::size_t copy(ReadStream&& src, WriteStream&& dest, std::error_code& ec)
{
    std::size_t total_bytes_copied = 0;
    std::array<std::uint8_t, 1024> buf;
    ec.clear();

    while (true) {
        auto bytes_read =  io::read(src, io::buffer(buf), ec);
        bool done = false;

        if (ec) {
            if (ec == io::stream_errc::eof) {
                ec.clear();
                done = true;
            } else {
                break;
            }
        }

        auto bytes_written = io::write(dest, io::buffer(buf, bytes_read), ec);

        assert(bytes_written == bytes_read);
        total_bytes_copied += bytes_written;

        if (done) {
            break;
        }
    }

    return total_bytes_copied;
}

/// @overload
template <typename ReadStream, typename WriteStream,
          typename = std::enable_if_t<is_sync_read_stream_v<ReadStream> &&
                                      is_sync_write_stream_v<WriteStream>>>
std::size_t copy(ReadStream&& src, WriteStream&& dest)
{
    std::error_code ec;
    std::size_t bytes_copied = io::copy(std::forward<ReadStream>(src),
                                        std::forward<WriteStream>(dest),
                                        ec);
    if (ec) {
        throw std::system_error{ec};
    }
    return bytes_copied;
}


}

#endif // MODERN_IO_COPY_HPP
