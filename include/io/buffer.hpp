
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

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

using net::is_mutable_buffer_sequence_v;
using net::is_const_buffer_sequence_v;
using net::is_dynamic_buffer_v;

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

using net::write;

}

#endif
