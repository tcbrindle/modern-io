
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_POSIX_MMAP_FILE_HPP
#define IO_POSIX_MMAP_FILE_HPP

#include <io/posix/file.hpp>
#include <io/memory_stream.hpp>

#include <sys/mman.h>

#include <cassert>

namespace io {
namespace posix {

namespace detail {

struct mmap_handle {

    static mmap_handle create(const file_descriptor_handle& fd, ::off_t size,
                              io::open_mode mode,
                              std::error_code& ec)
    {
        ec.clear();

        int prot = PROT_NONE;

        if ((mode & open_mode::read_only) != open_mode(0)) {
            prot = PROT_READ;
        } else if ((mode & open_mode::write_only) != open_mode(0)) {
            prot = PROT_WRITE;
        } else if ((mode & open_mode::read_write) != open_mode(0)) {
            prot = PROT_READ | PROT_WRITE;
        }

        void* addr = ::mmap(nullptr, size, prot,
                            MAP_FILE | MAP_SHARED,
                            fd.get(), 0);

        if (addr == MAP_FAILED) {
            ec.assign(errno, std::system_category());
            return mmap_handle{};
        }

        return mmap_handle(addr, size);
    }

    mmap_handle() = default;

    ~mmap_handle()
    {
        if (addr_ != nullptr) {
#if NDEBUG
            ::munmap(addr_, size_);
#else // !NDEBUG
            int ret = ::munmap(addr_, size_);
            assert(ret == 0);
#endif // NDEBUG
        }
    }

    mmap_handle(mmap_handle&& other)
            : addr_(other.addr_), size_(other.size_)
    {
        other.addr_ = nullptr;
        other.size_ = 0;
    }

    mmap_handle& operator=(mmap_handle&& other)
    {
        if (&other != this) {
            std::swap(addr_, other.addr_);
            std::swap(size_, other.size_);
        }
        return *this;
    }

    void* address() const { return addr_; }

    ::off_t size() const { return size_; }

    void unmap(std::error_code& ec)
    {
        ec.clear();
        errno = 0;
        if (::munmap(addr_, size_) != 0) {
            ec.assign(errno, std::system_category());
        } else {
            addr_ = nullptr;
            size_ = 0;
        }
    }

private:

    mmap_handle(void* addr, ::off_t size)
            : addr_(addr), size_(size) {}

    void* addr_ = nullptr;
    ::off_t size_ = 0;
};

} // end namespace detail

class mmap_file : public io::detail::memory_stream_impl<mmap_file, ::off_t> {
public:
    using size_type = ::off_t;

    mmap_file() = default;

    mmap_file(const io_std::filesystem::path& path,
              open_mode mode,
              io_std::filesystem::perms create_perms = default_creation_perms)
    {
        this->open(path, mode, create_perms);
    }

    void open(const io_std::filesystem::path& path,
              open_mode mode,
              io_std::filesystem::perms create_perms = default_creation_perms)
    {
        std::error_code ec;
        this->open(path, mode, create_perms, ec);
        if (ec) {
            throw std::system_error{ec};
        }
    }

    void open(const io_std::filesystem::path& path,
              open_mode mode,
              io_std::filesystem::perms create_perms,
              std::error_code& ec) noexcept
    {
        ec.clear();
        errno = 0;

        // First, try to open the file
        int raw_fd = ::open(path.c_str(), detail::open_mode_to_posix_mode(mode),
                            static_cast<::mode_t>(create_perms));
        if (raw_fd < 0) {
            ec.assign(errno, std::system_category());
            return;
        }
        file_descriptor_handle new_fd{raw_fd};

        // Now seek to the end to find the file size
        offset_type size = ::lseek(new_fd.get(), 0, SEEK_END);
        if (size == -1) {
            ec.assign(errno, std::system_category());
            return;
        }

        auto mmap = detail::mmap_handle::create(new_fd, size, mode, ec);
        if (ec) {
            return;
        }

        this->mmap_ = std::move(mmap);
    }

    void close()
    {
        std::error_code ec;
        this->close(ec);
        if (ec) {
            throw std::system_error{ec};
        }
    }

    void close(std::error_code& ec)
    {
        mmap_.unmap(ec);
    }

    template <typename ConstBufSeq,
              CONCEPT_REQUIRES_(ConstBufferSequence<ConstBufSeq>())>
    std::size_t write_some(const ConstBufSeq& cb)
    {
        std::error_code ec;
        std::size_t bytes_written = this->write_some(cb, ec);
        if (ec) {
            throw std::system_error{ec};
        }
        return bytes_written;
    }

    template <typename ConstBufSeq,
              CONCEPT_REQUIRES_(ConstBufferSequence<ConstBufSeq>())>
    std::size_t write_some(const ConstBufSeq& cb, std::error_code& ec)
    {
        ec.clear();
        errno = 0;

        auto buf = io::buffer(data(), size()) + get_position();

        auto total_bytes_written = io::buffer_copy(buf, cb);
        this->seek(total_bytes_written, io::seek_mode::current, ec);

        return total_bytes_written;
    }

    void* data() const noexcept { return mmap_.address(); }

    size_type size() const noexcept { return mmap_.size(); }

private:
    mmap_file(detail::mmap_handle mmap)
            : mmap_(std::move(mmap))
    {}

    detail::mmap_handle mmap_{};
};

} // namespace posix
} // end namespace io

// FIXME: This is ugly
#include "mmap_stream_reader.hpp"

#endif // IO_POSIX_MMAP_FILE_HPP
