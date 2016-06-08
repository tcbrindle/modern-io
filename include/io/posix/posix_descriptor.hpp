
#ifndef IO_POSIX_DESCRIPTOR_HPP
#define IO_POSIX_DESCRIPTOR_HPP

#include <unistd.h>

#include <cassert>

namespace io {

/// Move-only RAII wrapper around a Posix file descriptor
struct posix_descriptor
{
    /// Default-construct an invalid file descriptor
    posix_descriptor() = default;

    /// Create a `posix_descriptor` wrapping `fd`, optionally taking ownership
    /// @param fd The file descriptor
    /// @param transfer_ownership Whether to take ownership of the file descriptor
    explicit posix_descriptor(int fd, bool transfer_ownership = true) noexcept
            : fd_(fd), delete_(transfer_ownership)
    {}

    /// Move-construct from another posix_descriptor, taking ownership
    posix_descriptor(posix_descriptor&& other) noexcept
            : fd_(other.fd_),
              delete_(other.delete_)
    {
        other.fd_ = -1;
        other.delete_ = false;
    }

    /// Destroy the posix_descriptor
    /// If the underlying fd is owned, this will call ::close()
    ~posix_descriptor() noexcept
    {
        if (delete_) {
            int close_return = ::close(fd_);
            assert(close_return == 0);
        }
    }

    /// Return the value of the fd
    int get() const noexcept { return fd_; }

private:
    int fd_ = -1;
    bool delete_ = false;
};

}

#endif // IO_POSIX_DESCRIPTOR_HPP
