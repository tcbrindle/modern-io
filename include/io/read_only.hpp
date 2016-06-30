
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_READ_ONLY_HPP
#define IO_READ_ONLY_HPP

#include <io/basic_adaptor.hpp>

namespace io {

/// Adaptor which turns a read-write stream into a read-only stream
template <typename Stream>
struct read_only : basic_adaptor<Stream>
{
    using basic_adaptor<Stream>::basic_adaptor;

    /// Explicitly delete the write_some() member function to prevent writing
    template <typename ConstBufSeq>
    std::size_t write_some(const ConstBufSeq&) = delete;

    /// Explicitly delete the write_some() member function to prevent writing
    template <typename ConstBufSeq>
    std::size_t write_some(const ConstBufSeq&, std::error_code) = delete;
};

}

#endif // IO_READ_ONLY_HPP
