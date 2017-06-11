
// Adapted from asio/buffered_read_stream.hpp
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Modifications copyright (c) 2017 Tristan Brindle (tcbrindle at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MODERN_IO_BUFFERED_READ_STREAM_HPP_INCLUDED
#define MODERN_IO_BUFFERED_READ_STREAM_HPP_INCLUDED

#include <io/buffer.hpp>
#include <io/detail/buffered_stream_storage.hpp>
#include <io/detail/buffer_resize_guard.hpp>
#include <io/read.hpp>
#include <io/traits.hpp>

namespace io {

template <typename Stream, typename Allocator = std::allocator<unsigned char>>
struct buffered_read_stream
{
    using allocator_type = Allocator;
    using next_layer_type = std::remove_reference_t<Stream>;
    using buffer_type = detail::buffered_stream_storage<allocator_type>;
    using size_type = typename buffer_type::size_type;

    static constexpr std::size_t default_buffer_size = 1024;

    template <typename S,
              typename = std::enable_if_t<std::is_default_constructible<S>::value>>
    buffered_read_stream() {};

    template <typename S,
            typename = std::enable_if_t<std::is_default_constructible<S>::value>>
    explicit buffered_read_stream(const allocator_type& allocator)
        : storage_{default_buffer_size, allocator}
    {};

    explicit buffered_read_stream(Stream base)
        : base_(std::move(base))
    {}

    buffered_read_stream(Stream base, size_type buffer_size)
        : base_(std::move(base)),
          storage_(buffer_size)
    {}

    buffered_read_stream(const Stream& base, size_type buffer_size,
                         const allocator_type& allocator)
            : base_(base),
              storage_(buffer_size, allocator)
    {}

    template <typename S = Stream,
              typename = std::enable_if_t<!std::is_reference<Stream>::value>>
    buffered_read_stream(Stream&& base, size_type buffer_size,
                        const allocator_type& allocator)
        : base_(std::move(base)),
          storage_(buffer_size, allocator)
    {}

    next_layer_type& next_layer()
    {
        return base_;
    }

    /* SyncReadStream implementation */

    template <typename MutBufSeq>
    size_type read_some(const MutBufSeq& mb)
    {
        if (io::buffer_size(mb) == 0) {
            return 0;
        }

        if (storage_.empty()) {
            // if the buffer size we have been given is >= our buffer capacity,
            // avoid the fill and just copy straight through
            if (io::buffer_size(mb) >= storage_.capacity()) {
                return base_.read_some(mb);
            }
            this->fill();
        }

        return this->copy(mb);
    }

    template <typename MutBufSeq>
    size_type read_some(const MutBufSeq& mb, std::error_code& ec)
    {
        ec.clear();

        if (io::buffer_size(mb) == 0) {
            return 0;
        }

        if (storage_.empty()) {
            if (io::buffer_size(mb) >= storage_.capacity()) {
                return base_.read_some(mb, ec);
            }
            if (this->fill(ec) == 0) {
                return 0;
            }
        }

        return this->copy(mb);
    }

    template <typename MutBufSeq>
    size_type peek(const MutBufSeq& mb)
    {
        if (storage_.empty()) {
            this->fill();
        }

        return this->peek_copy(mb);
    }

    template <typename MutBufSeq>
    size_type peek(const MutBufSeq& mb, std::error_code& ec)
    {
        ec.clear();
        if (storage_.empty() && !this->fill(ec)) {
            return 0;
        }

        return this->peek_copy(mb);
    }


    /* SyncWriteStream implementation */
    template <typename ConstBufSeq, typename S = next_layer_type,
              typename = std::enable_if_t<io::is_sync_write_stream_v<S>>>
    std::size_t write_some(const ConstBufSeq& cb)
    {
        return base_.write_some(cb);
    }

    template <typename ConstBufSeq, typename S = next_layer_type,
              typename = std::enable_if_t<io::is_sync_write_stream_v<S>>>
    std::size_t write_some(const ConstBufSeq cb, std::error_code& ec)
    {
        return base_.write_some(cb, ec);
    }

    size_type fill()
    {
        detail::buffer_resize_guard<buffer_type> resize_guard(storage_);
        auto prev_size = storage_.size();
        storage_.resize(storage_.capacity()); // ?
        storage_.resize(prev_size + base_.read_some(io::buffer(
                storage_.data() + prev_size,
                storage_.size() + prev_size
        )));
        resize_guard.commit();
        return storage_.size() - prev_size;
    }

    size_type fill(std::error_code& ec)
    {
        detail::buffer_resize_guard<buffer_type> resize_guard(storage_);
        auto prev_size = storage_.size();
        storage_.resize(storage_.capacity()); // ?
        storage_.resize(prev_size +
                base_.read_some(io::buffer(
                storage_.data() + prev_size,
                storage_.size() + prev_size),
                                ec
        ));
        resize_guard.commit();
        return storage_.size() - prev_size;
    }

    io::position_type<next_layer_type>
    seek(io::offset_type<next_layer_type> distance, io::seek_mode from)
    {
        storage_.clear();
        return base_.seek(distance, from);
    }

    io::position_type<next_layer_type>
    seek(io::offset_type<next_layer_type> distance, io::seek_mode from,
         std::error_code& ec)
    {
        storage_.clear();
        return base_.seek(distance, from, ec);
    }

private:
    template <typename MutBufSeq>
    size_type copy(const MutBufSeq& mb)
    {
        auto bytes_copied = io::buffer_copy(mb, storage_.data(), storage_.size());
        storage_.consume(bytes_copied);
        return bytes_copied;
    }

    template <typename MutBufSeq>
    size_type peek_copy(const MutBufSeq& mb)
    {
        return io::buffer_copy(mb, storage_.data(), storage_.size());
    }

    Stream base_{};
    buffer_type storage_{default_buffer_size};
};




}

#endif
