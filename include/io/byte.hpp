
// Copyright (c) 2017 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MODERN_IO_BYTE_HPP_INCLUDED
#define MODERN_IO_BYTE_HPP_INCLUDED

#include <cstddef>

namespace io {

#if defined(_MSC_VER) && (_MSC_VER >= 1911) && (defined(_HAVE_STD_BYTE) || _HAVE_STD_BYTE)
using byte = std::byte;
using std::to_integer;
#elif defined(__GNUC__) && (__cplusplus >= 201703L)
using byte = std::byte;
using std::to_integer;
#else
using byte = unsigned char;

template <typename I,
          typename = std::enable_if_t<std::is_integral<I>::value>>
constexpr I to_integer(unsigned char c) { return c; }
#endif

}

#endif


