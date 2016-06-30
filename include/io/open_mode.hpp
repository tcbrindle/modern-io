
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_OPEN_MODE_HPP
#define IO_OPEN_MODE_HPP

#include <io/io_std/filesystem.hpp>

#include <type_traits>

namespace io {

/// Flags to specify modes when opening a file
enum class open_mode {
    /// The file will be opened for reading only.
    /// Attempting to write to the file will result in a runtime error.
    /// It is an error to set this flag and `write_only` at the same time.
    /// This is equivalent to the Posix mode `O_RDONLY`
    read_only = 1,
    /// The file will be opened for writing only.
    /// Attempting to read from the file will result in a runtime error.
    /// It is an error to set this flag and `read_only` at the same time.
    /// This is equivalent to the Posix mode `O_WRONLY`
    write_only = 2,
    /// Open the file for both reading and writing
    /// This is equivalent to the Posix mode `O_RDWR`
    read_write = 4,
    /// If the file exists, truncate it to zero size when opening
    /// This is equivalent to the Posix mode `O_TRUNC`
    truncate = 8,
    /// Always write to the end of the file, regardless of the current position
    /// of the stream cursor
    /// This is equivalent to the Posix mode `O_APPEND`
    append = 16,
    /// Create the file if it does not exist
    /// If this flag is *not* specified, a runtime error will occur if the file
    /// does not exist.
    /// This is equivalent to the Posix mode `O_CREAT`
    create = 32,
    /// Always create the file
    /// If this flag is specified, a runtime error will occur if the file
    /// already exists.
    /// This is equivalent to the Posix mode `O_CREAT | O_EXCL`
    always_create = 64
};

/// Bitwise `and` operation for `open_mode`
constexpr open_mode operator&(open_mode lhs, open_mode rhs)
{
    using u = std::underlying_type_t<open_mode>;
    return static_cast<open_mode>(static_cast<u>(lhs) & static_cast<u>(rhs));
}

/// Bitwise `or` operation for `open_mode`
constexpr open_mode operator|(open_mode lhs, open_mode rhs)
{
    using u = std::underlying_type_t<open_mode>;
    return static_cast<open_mode>(static_cast<u>(lhs) | static_cast<u>(rhs));
}

/// Bitwise `and` assignment operator for `open_mode`
constexpr open_mode& operator&=(open_mode& lhs, open_mode rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

/// Bitwise `or` assignment operator for `open_mode`
constexpr open_mode& operator|=(open_mode& lhs, open_mode rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

/// Default permissions when creating a new file, if not overridden
constexpr io_std::filesystem::perms default_creation_perms =
    io_std::filesystem::perms::owner_read |
    io_std::filesystem::perms::owner_write;

}

#endif // IO_OPEN_MODE_HPP
