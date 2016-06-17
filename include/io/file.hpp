
#ifndef IO_FILE_HPP
#define IO_FILE_HPP

#include <unistd.h>

#include <system_error>

#include <io/io_std/filesystem.hpp>
#include <io/io_concepts.hpp>
#include <io/seek.hpp>
#include <io/posix/file.hpp>

namespace io {

namespace fs = io_std::filesystem;

using file = posix::file;

inline file open_file(const fs::path& path, std::error_code& ec) noexcept
{
    return file::open(path, ec);
}

inline file open_file(const fs::path& path)
{
    std::error_code ec{};
    auto f = open_file(path, ec);
    if (ec) {
        throw std::system_error{ec};
    }
    return f;
}

} // end namespace io

#endif // IO_FILE_HPP