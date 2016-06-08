
#ifndef IO_SEEK_HPP
#define IO_SEEK_HPP

#include <io/io_concepts.hpp>

namespace io {

enum class seek_mode {
    start,
    end,
    current
};

template <typename Stream,
          CONCEPT_REQUIRES_(SeekableStream<Stream>())>
using offset_type = typename Stream::offset_type;

template <typename Stream,
          CONCEPT_REQUIRES_(SeekableStream<Stream>())>
offset_type<Stream>
seek(Stream& stream,
     offset_type<Stream> offset,
     seek_mode from,
     std::error_code& ec)
{
    return stream.seek(offset, from, ec);
}

template <typename Stream,
          CONCEPT_REQUIRES_(SeekableStream<Stream>())>
offset_type<Stream>
seek(Stream& stream,
     offset_type<Stream> offset,
     seek_mode from)
{
    return stream.seek(offset, from);
}

template <typename Stream,
          CONCEPT_REQUIRES_(SeekableStream<Stream>())>
offset_type<Stream>
get_position(Stream& stream, std::error_code& ec)
{
    return stream.get_position(ec);
}

template <typename Stream,
          CONCEPT_REQUIRES_(SeekableStream<Stream>())>
offset_type<Stream>
get_position(Stream& stream)
{
    return stream.get_position();
}

} // end namespace io

#endif
