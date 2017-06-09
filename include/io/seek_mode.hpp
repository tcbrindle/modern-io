
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_SEEK_MODE_HPP
#define IO_SEEK_MODE_HPP

#include <io/traits.hpp>

namespace io {

enum class seek_mode {
    start,
    end,
    current
};

} // end namespace io

#endif // IO_SEEK_MODE_HPP
