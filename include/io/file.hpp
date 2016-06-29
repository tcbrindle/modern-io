
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

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

inline file open_file(const fs::path& path,
                      open_mode mode,
                      fs::perms create_perms,
                      std::error_code& ec) noexcept
{
    file f;
    f.open(path, mode, create_perms, ec);
    return f;
}


inline file open_file(const fs::path& path,
                      open_mode mode,
                      std::error_code& ec) noexcept
{
    return open_file(path, mode, default_creation_perms, ec);
}

inline file open_file(const fs::path& path,
                      open_mode mode,
                      fs::perms perms = default_creation_perms)
{
    std::error_code ec{};
    auto f = open_file(path, mode, perms, ec);
    if (ec) {
        throw std::system_error{ec};
    }
    return f;
}

} // end namespace io

#endif // IO_FILE_HPP