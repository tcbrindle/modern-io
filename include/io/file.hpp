
#ifndef IO_FILE_HPP
#define IO_FILE_HPP

#include <unistd.h>

#include <system_error>

#include <io/io_concepts.hpp>
#include <io/seek.hpp>
#include <io/posix/posix_file.hpp>

namespace io {

using file = posix_file;

inline file open_file(const char* path, std::error_code& ec) noexcept
{
    return file::open(path, ec);
}

inline file open_file(const char* path)
{
    std::error_code ec{};
    auto f = open_file(path, ec);
    if (ec) {
        throw std::system_error{ec};
    }
    return f;
}

inline file open_file(const std::string& path, std::error_code& ec) noexcept
{
    return open_file(path.c_str(), ec);
}

inline file open_file(const std::string& path)
{
    return open_file(path.c_str());
}

#ifdef IO_HAVE_STD_FILESYSTEM

inline file open_file(const fs::path& path, std::error_code& ec) noexcept
{
    return open_file(path.c_str(), ec);
}

inline file open_file(const fs::path& path)
{
    return open_file(path.c_str());
}

#endif // IO_HAVE_STD_FILESYSTEM

} // end namespace io

#endif // IO_FILE_HPP