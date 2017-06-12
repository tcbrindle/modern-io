
// Copyright (c) 2017 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MODERN_IO_OPTIONAL_HPP_INCLUDED
#define MODERN_IO_OPTIONAL_HPP_INCLUDED

#ifdef __has_include
    #if __has_include(<optional>)
        #define IO_HAVE_OPTIONAL 1
        #include <optional>
        namespace io_std {
            using std::optional;
            using std::nullopt;
        }
    #elif __has_include(<experimental/optional>)
        #define IO_HAVE_OPTIONAL 1
        #include <experimental/optional>
        namespace io_std {
            using std::experimental::optional;
            using std::experimental::nullopt;
        }
    #endif
#endif // __has_include

#if !IO_HAVE_OPTIONAL
#error "std::optional or std::experimental::optional required"
#endif

#endif
