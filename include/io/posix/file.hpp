
#ifndef IO_POSIX_FILE_HPP
#define IO_POSIX_FILE_HPP

#include <io/io_std/filesystem.hpp>
#include <io/posix/file_descriptor_handle.hpp>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

namespace io {
namespace posix {

namespace detail {
constexpr int seek_mode_to_whence_arg(seek_mode m)
{
    switch (m) {
    case seek_mode::start:
        return SEEK_SET;
    case seek_mode::end:
        return SEEK_END;
    case seek_mode::current:
        return SEEK_CUR;
    default:
        return -1;
    }
}

} // end namespace detail


class file {
public:
    using offset_type = ::off_t;
    using native_handle_type = int;

    static file open(const io_std::filesystem::path& path, std::error_code& ec) noexcept
    {
        ec.clear();
        errno = 0;
        file_descriptor_handle fd{::open(path.c_str(), O_RDWR | O_CREAT, 0600), true};

        if (fd.get() < 0) {
            ec.assign(errno, std::system_category());
            return posix::file{};
        }

        return posix::file{std::move(fd)};
    }

    file() = default;

    explicit file(posix::file_descriptor_handle fd) noexcept
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

    offset_type seek(offset_type offset, seek_mode from)
    {
        std::error_code ec;
        auto o = this->seek(offset, from, ec);
        if (ec) {
            throw std::system_error(ec);
        }
        return o;
    }

    offset_type
    seek(offset_type offset, seek_mode from, std::error_code& ec) noexcept
    {
        ec.clear();
        errno = 0;
        auto o = ::lseek(native_handle(), static_cast<off_t>(offset),
                         detail::seek_mode_to_whence_arg(from));

        if (o < 0) {
            ec.assign(errno, std::system_category());
            return offset_type{};
        }

        return static_cast<offset_type>(o);
    }

    void sync(std::error_code& ec) noexcept
    {
        ec.clear();
        if (fsync(this->native_handle()) != 0) {
            ec.assign(errno, std::system_category());
        }
    }

    void sync()
    {
        std::error_code ec{};
        this->sync(ec);
        if (ec) {
            throw std::system_error{ec};
        }
    }

private:
    file_descriptor_handle fd_{};
};

static_assert(SeekableStream<posix::file>(),
              "posix::file does not meet the SeekableStream requirements");

} // end namespace posix
} // end namespace io

#endif // IO_POSIX_FILE_HPP

