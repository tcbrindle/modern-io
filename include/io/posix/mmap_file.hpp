
#ifndef IO_POSIX_MMAP_FILE_HPP
#define IO_POSIX_MMAP_FILE_HPP

#include <io/posix/file.hpp>

#include <sys/mman.h>

#include <cassert>

namespace io {
namespace posix {

namespace detail {

struct mmap_handle {

    static mmap_handle create(file_descriptor_handle fd, ::off_t size,
                              std::error_code& ec)
    {
        ec.clear();
        void* addr = ::mmap(nullptr, size,
                            PROT_READ | PROT_WRITE,
                            MAP_FILE | MAP_SHARED,
                            fd.get(), 0);

        if (!addr) {
            ec.assign(errno, std::system_category());
            return mmap_handle{};
        }

        return mmap_handle(std::move(fd), addr, size);
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
            : fd_(std::move(other.fd_)), addr_(other.addr_), size_(other.size_)
    {
        other.addr_ = nullptr;
        other.size_ = 0;
    }

    void* address() const { return addr_; }

    ::off_t size() const { return size_; }

private:

    mmap_handle(file_descriptor_handle fd, void* addr, ::off_t size)
            : fd_(std::move(fd)), addr_(addr), size_(size) {}

    file_descriptor_handle fd_;
    void* addr_ = nullptr;
    ::off_t size_ = 0;
};

} // end namespace detail

class mmap_file {
public:
    using offset_type = ::off_t;

    static mmap_file open(const io_std::filesystem::path& path)
    {
        std::error_code ec;
        auto file = mmap_file::open(path, ec);
        if (ec) {
            throw std::system_error{ec};
        }
        return file;
    }

    static mmap_file open(const io_std::filesystem::path& path,
                          std::error_code& ec)
    {
        ec.clear();
        errno = 0;

        // First, try to open the file
        file_descriptor_handle fd{::open(path.c_str(), O_RDWR | O_CREAT, 0600),
                                  true};
        if (fd.get() < 0) {
            ec.assign(errno, std::system_category());
            return mmap_file{};
        }

        // Seek to the end to find the size
        offset_type size = ::lseek(fd.get(), 0, SEEK_END);
        if (size == -1) {
            ec.assign(errno, std::system_category());
            return mmap_file{};
        }

        auto mmap = detail::mmap_handle::create(std::move(fd), size, ec);
        if (ec) {
            return mmap_file{};
        }

        return mmap_file{std::move(mmap)};
    }

    mmap_file() = default;

    mutable_buffer data() const
    {
        return mutable_buffer(static_cast<char*>(mmap_.address()) + pos_,
                              mmap_.size() - pos_);
    }

    /// SyncReadStream implementation
    template <typename MutBufSeq,
            CONCEPT_REQUIRES_(MutableBufferSequence<MutBufSeq>())>
    std::size_t read_some(const MutBufSeq& mb)
    {
        std::error_code ec;
        std::size_t bytes_read = this->read_some(mb, ec);
        if (ec) {
            throw std::system_error{ec};
        }
        return bytes_read;
    }

    /// SyncReadStream implementation
    template <typename MutBufSeq,
            CONCEPT_REQUIRES_(MutableBufferSequence<MutBufSeq>())>
    std::size_t read_some(const MutBufSeq& mb, std::error_code& ec)
    {
        if (pos_ == mmap_.size()) {
            ec = make_error_code(stream_errc::eof);
            return 0;
        }

        std::size_t bytes_copied = buffer_copy(mb, data());
        pos_ += bytes_copied;

        assert(pos_ <= mmap_.size());

        return bytes_copied;
    }

private:
    mmap_file(detail::mmap_handle mmap)
            : mmap_(std::move(mmap)) {}

    detail::mmap_handle mmap_{};
    offset_type pos_ = 0;
};

} // namespace posix
} // end namespace io

// FIXME: This is ugly
#include "mmap_stream_reader.hpp"

#endif // IO_POSIX_MMAP_FILE_HPP
