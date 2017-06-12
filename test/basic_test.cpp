
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "catch.hpp"

#include <io/string_stream.hpp>
#include <io/byte_range.hpp>

#include <iostream>

TEST_CASE("String streams can be printed", "[string_stream]")
{
    auto stream = io::string_stream("Hello, world!\n");
    auto reader = io::read(stream);

    std::copy(std::begin(reader), std::end(reader),
              std::ostream_iterator<char>(std::cout));
}