
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_SEEK_HPP
#define IO_SEEK_HPP

#include <io/io_concepts.hpp>

namespace io {

enum class seek_mode {
    start,
    end,
    current
};

template <typename Stream>
using offset_t = decltype(std::declval<Stream>().seek(0, seek_mode::start));

template <typename Stream,
          CONCEPT_REQUIRES_(SeekableStream<Stream>())>
offset_t<Stream>
seek(Stream& stream,
     offset_t<Stream> offset,
     seek_mode from,
     std::error_code& ec)
{
    return stream.seek(offset, from, ec);
}

template <typename Stream,
          CONCEPT_REQUIRES_(SeekableStream<Stream>())>
offset_t<Stream>
seek(Stream& stream,
     offset_t<Stream> offset,
     seek_mode from)
{
    return stream.seek(offset, from);
}

template <typename Stream,
          CONCEPT_REQUIRES_(SeekableStream<Stream>())>
offset_t<Stream>
get_position(const Stream& stream, std::error_code& ec)
{
    return stream.get_position(ec);
}

template <typename Stream,
          CONCEPT_REQUIRES_(SeekableStream<Stream>())>
offset_t<Stream>
get_position(const Stream& stream)
{
    return stream.get_position();
}

} // end namespace io

#endif
