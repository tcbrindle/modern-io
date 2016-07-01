
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_WRITE_ONLY_HPP
#define IO_WRITE_ONLY_HPP

#include <io/basic_adaptor.hpp>

namespace io {

/// An adaptor which turns a read-write stream into a write-only stream
template <typename Stream>
struct write_only : basic_adaptor<Stream>
{
    using basic_adaptor<Stream>::basic_adaptor;

    /// Explicitly delete the read_some() member function to prevent reading
    template <typename MutBufSeq>
    std::size_t read_some(const MutBufSeq&) = delete;

    /// Explicitly delete the read_some() member function to prevent reading
    template <typename MutBufSeq>
    std::size_t read_some(const MutBufSeq&, std::error_code) = delete;
};

}

#endif // IO_WRITE_ONLY_HPP
