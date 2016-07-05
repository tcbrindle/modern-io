


#ifndef MODERN_IO_FILE_HPP
#define MODERN_IO_FILE_HPP

#define UNICODE

#include <io/open_mode.hpp>
#include <io/windows/handle.hpp>

#include <iostream>

namespace io {
namespace windows {
namespace detail {

constexpr DWORD seek_mode_to_windows_mode(seek_mode m)
{
    switch (m) {
        case seek_mode::start: return FILE_BEGIN;
        case seek_mode::current: return FILE_CURRENT;
        case seek_mode::end: return FILE_END;
    }
    return -1;
}

constexpr DWORD open_mode_to_desired_access(open_mode m)
{
    if ((m & open_mode::read_only) != static_cast<open_mode>(0)) {
        return  GENERIC_READ;
    }
    if ((m & open_mode::write_only) != static_cast<open_mode>(0)) {
        return GENERIC_WRITE;
    }
    if ((m & open_mode::read_write) != static_cast<open_mode>(0)) {
        return GENERIC_READ | GENERIC_WRITE;
    }
    return 0;
}

constexpr DWORD perms_to_shared_mode(io_std::filesystem::perms)
{
    // FIXME: Implement this properly
    return FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
}

constexpr DWORD open_mode_to_creation_disp(open_mode m)
{
    if ((m & open_mode::truncate) != static_cast<open_mode>(0)) {
        return  TRUNCATE_EXISTING;
    }
    if ((m & open_mode::always_create) != static_cast<open_mode>(0)) {
        return CREATE_ALWAYS;
    }
    if ((m & open_mode::create) != static_cast<open_mode>(0)) {
        return OPEN_ALWAYS;
    }
    // neither create nor always_create was specified, so open existing
    return OPEN_EXISTING;
}

} // end namespace detail

class file {
public:
    using offset_type = __int64;
    using native_handle_type = HANDLE;

    file() = default;

    file(const io_std::filesystem::path& path,
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

        HANDLE new_handle = CreateFileW(path.c_str(),
                              detail::open_mode_to_desired_access(mode),
                              detail::perms_to_shared_mode(create_perms),
                              nullptr, // SECURITY_ATTRIBUTES
                              detail::open_mode_to_creation_disp(mode),
                              FILE_ATTRIBUTE_NORMAL,
                              nullptr // Template handle
                              );
        
        if (new_handle == INVALID_HANDLE_VALUE) {
            ec.assign(::GetLastError(), std::system_category());
        } else {
            handle_ = handle_t{new_handle};
        }
    }

    void close()
    {
        std::error_code ec;
        this->close(ec);
        if (ec) {
            throw std::system_error{ec};
        }
    }

    void close(std::error_code& ec) noexcept
    {
        handle_.close(ec);
    }

    native_handle_type native_handle() const noexcept
    {
        return handle_.get();
    }

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
    std::size_t read_some(const MutBufSeq& mb, std::error_code& ec) noexcept;

    std::size_t read_some(const io::mutable_buffer& mb, std::error_code& ec)
    {
        DWORD bytes_read = 0;
        if (!::ReadFile(handle_.get(), mb.data(), mb.size(), &bytes_read, nullptr)) {
            ec.assign(::GetLastError(), std::system_category());
        } else if (bytes_read == 0) {
            ec = stream_errc::eof;
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
    std::size_t write_some(ConstBufSeq& cb, std::error_code& ec) noexcept;

    std::size_t write_some(const io::const_buffer& cb, std::error_code& ec) noexcept
    {
        DWORD bytes_written = 0;

        if (!::WriteFile(handle_.get(), cb.data(), cb.size(), &bytes_written, nullptr)) {
            ec.assign(::GetLastError(), std::system_category());
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
        LARGE_INTEGER l_offset{};
        l_offset.QuadPart = offset;
        LARGE_INTEGER new_pos{};
        bool success = ::SetFilePointerEx(handle_.get(), l_offset, &new_pos,
                                          detail::seek_mode_to_windows_mode(from));
        if (!success) {
            ec.assign(::GetLastError(), std::system_category());
        }
        return new_pos.QuadPart;
    }

    void sync(std::error_code& ec) noexcept
    {
        if (!::FlushFileBuffers(handle_.get())) {
            ec.assign(::GetLastError(), std::system_category());
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
    windows::handle_t handle_{};
};

static_assert(SeekableStream<windows::file>(),
              "windows::file does not meet the SeekableStream requirements");

} // end namespace windows
} // end namespace io

#endif //MODERN_IO_FILE_HPP
