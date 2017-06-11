
// Adapted from asio/buffered_stream.hpp
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Modifications copyright (c) 2017 Tristan Brindle (tcbrindle at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MODERN_IO_BUFFERED_STREAM_HPP_INCLUDED
#define MODERN_IO_BUFFERED_STREAM_HPP_INCLUDED

#include <io/buffered_read_stream.hpp>
#include <io/buffered_write_stream.hpp>

namespace io {

template <typename Stream, typename Allocator = std::allocator<unsigned char>>
struct buffered_stream {
    using allocator_type = Allocator;
    using base_stream_type = Stream;


    template <typename S = base_stream_type,
              std::enable_if_t<std::is_default_constructible<S>::value>>
    buffered_stream()
    {}

    template <typename S = base_stream_type,
            std::enable_if_t<std::is_default_constructible<S>::value>>
    explicit buffered_stream(const allocator_type& allocator)
            : write_stream_{allocator},
              read_stream_(write_stream_, allocator)
    {}

    explicit buffered_stream(base_stream_type base)
        : write_stream_(std::move(base))
    {}

    buffered_stream(base_stream_type base,
                    std::size_t read_buffer_size,
                    std::size_t write_buffer_size)
        : buffered_stream(std::move(base), read_buffer_size,
                          write_buffer_size, allocator_type{})
    {}

    buffered_stream(base_stream_type base,
                    std::size_t read_buffer_size,
                    std::size_t write_buffer_size,
                    const allocator_type& allocator)
        : write_stream_(std::move(base), write_buffer_size, allocator),
          read_stream_(write_stream_, read_buffer_size, allocator)
    {}

    /* BufferedSyncReadStream implementation */
    template <typename MutBufSeq>
    std::size_t read_some(const MutBufSeq& mb)
    {
        return read_stream_.read_some(mb);
    }

    template <typename MutBufSeq>
    std::size_t read_some(const MutBufSeq& mb, std::error_code& ec)
    {
        return read_stream_.read_some(mb, ec);
    }

    template <typename MutBufSeq>
    std::size_t peek(const MutBufSeq& mb)
    {
        return read_stream_.peek(mb);
    }

    template <typename MutBufSeq>
    std::size_t peek(const MutBufSeq& mb, std::error_code& ec)
    {
        return read_stream_.peek(mb, ec);
    }

    std::size_t fill()
    {
        return read_stream_.fill();
    }

    std::size_t fill(std::error_code& ec)
    {
        return read_stream_.fill(ec);
    }

    /* BufferedSyncWriteStream implementation */

    template <typename ConstBufSeq>
    std::size_t write_some(const ConstBufSeq& cb)
    {
        return read_stream_.write_some(cb);
    }

    template <typename ConstBufSeq>
    std::size_t write_some(const ConstBufSeq& cb, std::error_code& ec)
    {
        return read_stream_.write_some(cb, ec);
    }

    std::size_t flush()
    {
        return read_stream_.next_layer().flush();
    }

    std::size_t flush(std::error_code& ec)
    {
        return read_stream_.next_layer().flush(ec);
    }

    /* SeekableStream implementation */
    io::position_type<next_layer_type>
    seek(io::offset_type<next_layer_type> distance, io::seek_mode from)
    {
        read_stream_.seek(distance, from);
    }

    io::position_type<next_layer_type>
    seek(io::offset_type<next_layer_type> distance, io::seek_mode from,
         std::error_code& ec)
    {
        read_stream_.seek(distance, from, ec);
    }

private:
    using write_stream_type = io::buffered_write_stream<base_stream_type>;
    write_stream_type write_stream_;

    using read_stream_type = io::buffered_read_stream<write_stream_type&>;
    read_stream_type read_stream_{write_stream_};
};

}

#endif
