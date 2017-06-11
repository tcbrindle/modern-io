
// Adapted from asio/buffered_write_stream.hpp
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Modifications copyright (c) 2017 Tristan Brindle (tcbrindle at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MODERN_IO_BUFFERED_WRITE_STREAM_HPP_INCLUDED
#define MODERN_IO_BUFFERED_WRITE_STREAM_HPP_INCLUDED

#include <io/buffer.hpp>
#include <io/detail/buffered_stream_storage.hpp>
#include <io/detail/buffer_resize_guard.hpp>
#include <io/read.hpp>
#include <io/traits.hpp>

namespace io {

template <typename Stream, typename Allocator = std::allocator<unsigned char>>
struct buffered_write_stream {
    using allocator_type = Allocator;
    using next_layer_type = std::remove_reference_t<Stream>;
    using buffer_type = detail::buffered_stream_storage<allocator_type>;
    using size_type = typename buffer_type::size_type;

    static constexpr std::size_t default_buffer_size = 1024;

    template <typename S = Stream,
              typename = std::enable_if_t<std::is_default_constructible<S>::value>>
    buffered_write_stream() {};

    template <typename S = Stream,
            typename = std::enable_if_t<std::is_default_constructible<S>::value>>
    explicit buffered_write_stream(const allocator_type& allocator)
            : storage_{default_buffer_size, allocator}
    {};

    explicit buffered_write_stream(Stream base)
        : base_(std::move(base))
    {}

    buffered_write_stream(Stream base, size_type buffer_size)
        : base_(std::move(base)),
    storage_(buffer_size)
    {}

    buffered_write_stream(Stream base, size_type buffer_size,
                         const allocator_type& allocator)
        : base_(std::move(base)),
          storage_(buffer_size, allocator)
    {}

    ~buffered_write_stream()
    {
        std::error_code ec;
        this->flush(ec);
        // Swallow errors -- there's nothing else we can do
    }

    next_layer_type& next_layer()
    {
        return base_;
    }

    /* Pass-through SyncReadStream implementation */
    template <typename MutBufSeq, typename S = next_layer_type,
              typename = std::enable_if_t<io::is_sync_read_stream_v<S>>>
    std::size_t read_some(const MutBufSeq& mb)
    {
        return base_.read_some(mb);
    }

    template <typename MutBufSeq, typename S = next_layer_type,
            typename = std::enable_if_t<io::is_sync_read_stream_v<S>>>
    std::size_t read_some(const MutBufSeq& mb, std::error_code& ec)
    {
        return base_.read_some(mb, ec);
    }

    /* SyncWriteStream implementation */
    template <typename ConstBufSeq>
    std::size_t write_some(const ConstBufSeq& cb)
    {
        if (io::buffer_size(cb) == 0) {
            return 0;
        }

        if (storage_.size() == storage_.capacity()) {
            this->flush();
        }

        return this->copy(cb);
    }

    template <typename ConstBufSeq>
    std::size_t write_some(const ConstBufSeq& cb, std::error_code& ec)
    {
        ec.clear();

        if (io::buffer_size(cb) == 0) {
            return 0;
        }

        if (storage_.size() == storage_.capacity() && !this->flush(ec)) {
            return 0;
        }

        return this->copy(cb);
    }

    std::size_t flush()
    {
        const auto bytes_written = io::write(base_, io::buffer(
                storage_.data(), storage_.size()));
        storage_.consume(bytes_written);
        return bytes_written;
    }

    void flush(std::error_code& ec)
    {
        const auto bytes_written = io::write(base_, io::buffer(
                storage_.data(), storage_.size()), io::transfer_all{}, ec);
        storage_.consume(bytes_written);
        return bytes_written;
    }

    io::position_type<next_layer_type>
    seek(io::offset_type<next_layer_type> distance, io::seek_mode from)
    {
        flush();
        return base_.seek(distance, from);
    }

    io::position_type<next_layer_type>
    seek(io::offset_type<next_layer_type> distance, io::seek_mode from,
         std::error_code& ec)
    {
        ec.clear();
        flush(ec);
        if (ec) {
            return 0;
        }
        return base_.seek(distance, from, ec);
    }

private:
    template <typename ConstBufSeq>
    std::size_t copy(const ConstBufSeq& cb)
    {
        const auto orig_size = storage_.size();
        const auto space_avail = storage_.capacity() - orig_size;
        const auto bytes_avail = io::buffer_size(cb);
        const auto length = std::min(bytes_avail, space_avail);
        storage_.resize(orig_size + length);
        return io::buffer_copy(storage_.data() + orig_size, cb, length);
    }

    Stream base_;
    buffer_type storage_{default_buffer_size};
};

}

#endif
