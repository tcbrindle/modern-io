
#ifndef IO_POSIX_DESCRIPTOR_HPP
#define IO_POSIX_DESCRIPTOR_HPP

#include <unistd.h>

#include <cassert>

namespace io {
namespace posix {

/// Move-only RAII wrapper around a Posix file file_descriptor_handle
struct file_descriptor_handle {
    /// Default-construct an invalid file file_descriptor_handle
    file_descriptor_handle() = default;

    /// Create a `file_descriptor_handle` wrapping `fd`, optionally taking ownership
    /// @param fd The file descriptor
    /// @param transfer_ownership Whether to take ownership of the file descriptor
    explicit file_descriptor_handle(int fd, bool transfer_ownership = true) noexcept
            : fd_(fd), delete_(transfer_ownership) {}

    /// Move-construct from another `file_descriptor_handle`, taking ownership
    file_descriptor_handle(file_descriptor_handle&& other) noexcept
        : fd_(other.fd_), delete_(other.delete_)
    {
        other.fd_ = -1;
        other.delete_ = false;
    }

    file_descriptor_handle& operator=(file_descriptor_handle&& other) noexcept
    {
        if (&other != this) {
            std::swap(fd_, other.fd_);
            std::swap(delete_, other.delete_);
        }
        return *this;
    }

    /// Destroy the `file_descriptor_handle`
    /// If the underlying fd is owned, this will call ::close()
    ~file_descriptor_handle() noexcept
    {
        if (delete_) {
#ifdef NDEBUG
            ::close(fd_);
#else // !NDEBUG
            int close_return = ::close(fd_);
            assert(close_return == 0);
#endif // NDEBUG
        }
    }

    /// Return the value of the fd
    int get() const noexcept { return fd_; }

    /// Manually close the file descriptor
    void close(std::error_code& ec) noexcept
    {
        ec.clear();
        errno = 0;
        if (::close(fd_) == 0) {
            fd_ = -1;
            delete_ = false;
        } else {
            ec.assign(errno, std::system_category());
        }
    }

private:
    int fd_ = -1;
    bool delete_ = false;
};

} // end namespace posix
} // end namespace io

#endif // IO_POSIX_DESCRIPTOR_HPP
