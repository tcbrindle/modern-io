
#ifndef IO_BUFFER_HPP
#define IO_BUFFER_HPP

#include <io/net/buffer.hpp>

namespace io {

using net::stream_errc;
using net::stream_category;
using net::make_error_code;
using net::make_error_condition;

using net::mutable_buffer;
using net::const_buffer;

using net::is_mutable_buffer_sequence;
using net::is_const_buffer_sequence;
using net::is_dynamic_buffer;

using net::buffer_sequence_begin;
using net::buffer_sequence_end;

using net::buffer_size;
using net::buffer_copy;

using net::buffer;

using net::dynamic_vector_buffer;
using net::dynamic_string_buffer;
using net::dynamic_buffer;

using net::transfer_all;
using net::transfer_at_least;
using net::transfer_exactly;

using net::read;
using net::write;

using net::read_until;

}

#endif
