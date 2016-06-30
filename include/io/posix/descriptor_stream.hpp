
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Contains code from Asio, Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_POSIX_DESCRIPTOR_STREAM_HPP
#define IO_POSIX_DESCRIPTOR_STREAM_HPP

#include <io/buffer.hpp>
#include <io/io_concepts.hpp>
#include <io/posix/file_descriptor_handle.hpp>

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

namespace io {
namespace posix {

struct descriptor_stream {
    using native_handle_type = int;

    descriptor_stream() noexcept = default;

    descriptor_stream(file_descriptor_handle fd) noexcept
            : fd_{std::move(fd)} {}

    native_handle_type native_handle() const noexcept { return fd_.get(); }

    // SyncReadStream implementation

    template <typename MutBufSeq,
            CONCEPT_REQUIRES_(MutableBufferSequence<MutBufSeq>())>
    std::size_t read_some(const MutBufSeq& mb)
    {
        std::error_code ec;
        auto sz = this->read_some(mb, ec);
        if (ec) {
            throw std::system_error{ec};
        }
        return sz;
    }

    template <typename MutBufSeq,
            CONCEPT_REQUIRES_(MutableBufferSequence<MutBufSeq>())>
    std::size_t read_some(const MutBufSeq& mb, std::error_code& ec) noexcept
    {
        ec.clear();

        if (buffer_size(mb) == 0) {
            return 0;
        }

        // Prepare buffers
        constexpr int max_iovec = 16; //?
        std::array<::iovec, max_iovec> io_vecs{};

        int i = 0;
        for (auto it = buffer_sequence_begin(mb);
             it != buffer_sequence_end(mb); ++it) {
            io_vecs[i].iov_base = it->data();
            io_vecs[i].iov_len = it->size();
            if (++i == max_iovec) {
                break;
            }
        }

        auto bytes = ::readv(native_handle(), io_vecs.data(), i);

        if (bytes == 0) {
            ec = stream_errc::eof;
        }
        else if (bytes < 0) {
            ec.assign(errno, std::system_category());
        }
        else {
            ec.clear();
        }

        return bytes;
    }

    std::size_t read_some(const io::mutable_buffer& mb, std::error_code& ec)
    {
        ec.clear();

        if (mb.size() == 0) {
            return 0;
        }

        auto bytes_read = ::read(native_handle(), mb.data(), mb.size());

        if (bytes_read == 0) {
            ec = stream_errc::eof;
        }
        else if (bytes_read < 0) {
            ec.assign(errno, std::system_category());
        }

        return bytes_read;
    }

    // SyncWriteStream implementation

    template <typename ConstBufSeq,
            CONCEPT_REQUIRES_(ConstBufferSequence<ConstBufSeq>())>
    std::size_t write_some(ConstBufSeq& cb)
    {
        std::error_code ec;
        auto sz = write_some(cb, ec);

        if (ec) {
            throw std::system_error{ec};
        }

        return sz;
    }

    template <typename ConstBufSeq,
            CONCEPT_REQUIRES_(ConstBufferSequence<ConstBufSeq>())>
    std::size_t write_some(ConstBufSeq& cb, std::error_code& ec) noexcept
    {
        if (io::buffer_size(cb) == 0) {
            ec.clear();
            return 0;
        }

        // Prepare buffers
        constexpr int max_iovec = 16; //?
        std::array<::iovec, max_iovec> io_vecs{};

        int i = 0;
        for (auto it = buffer_sequence_begin(cb);
             it != buffer_sequence_end(cb); ++it) {
            io_vecs[i].iov_base = it->data();
            io_vecs[i].iov_len = it->size();
            if (++i == max_iovec) {
                break;
            }
        }

        auto bytes_written = ::writev(native_handle(), io_vecs.data(), i);

        if (bytes_written < 0) {
            ec.assign(errno, std::system_category());
        }

        return bytes_written;
    }

    std::size_t write_some(const io::mutable_buffer& cb, std::error_code& ec)
    {
        ec.clear();

        if (cb.size() == 0) {
            return 0;
        }

        auto bytes_written = ::write(native_handle(), cb.data(), cb.size());

        if (bytes_written < 0) {
            ec.assign(errno, std::system_category());
        }

        return bytes_written;
    }

private:
    template <typename MutBufSeq>
    std::size_t do_read_some(MutBufSeq&& mb, std::error_code& ec) noexcept;

    file_descriptor_handle fd_{};
};

static_assert(SyncReadStream<descriptor_stream>(),
              "posix_descriptor_stream does not meet the SyncReadStream requirements");

} // end namespace posix
} // end namespace io

#endif // IO_POSIX_DESCRIPTOR_STREAM_HPP