
#ifndef IO_POSIX_FILE_HPP
#define IO_POSIX_FILE_HPP

#include <io/posix/posix_descriptor_stream.hpp>

#include <fcntl.h>

namespace io {

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
    }
}

} // end namespace detail


struct posix_file : posix_descriptor_stream {
    using offset_type = ::off_t;

    static posix_file open(const char* path, std::error_code& ec) noexcept
    {
        ec.clear();
        errno = 0;
        posix_descriptor fd{::open(path, O_RDWR | O_CREAT, 0600), true};

        if (fd.get() < 0) {
            ec.assign(errno, std::system_category());
            return posix_file{};
        }

        return posix_file{std::move(fd)};
    }

    posix_file() = default;

    explicit posix_file(posix_descriptor fd) noexcept
            : posix_descriptor_stream{std::move(fd)}
    {}

    offset_type seek(offset_type offset, seek_mode from)
    {
        std::error_code ec;
        auto o = this->seek(offset, from, ec);
        if (ec) {
            throw std::system_error(ec);
        }
        return o;
    }

    offset_type seek(offset_type offset, seek_mode from, std::error_code& ec) noexcept
    {
        ec.clear();
        errno = 0;
        auto o = ::lseek(native_handle(), static_cast<off_t>(offset),
                         io::detail::seek_mode_to_whence_arg(from));

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
};

static_assert(SeekableStream<posix_file>(), "posix_file does not meet the SeekableStream requirements");

}

#endif // IO_POSIX_FILE_HPP

