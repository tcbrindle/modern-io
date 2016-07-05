
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_STD_STRING_VIEW_HPP
#define IO_STD_STRING_VIEW_HPP

#ifdef __has_include
    #if __has_include(<string_view>)
        #include <string_view>
        #define IO_HAVE_STRING_VIEW 1
        namespace io_std {
            using std::basic_string_view;
            using string_view = std::string_view;
            using u16string_view = std::u16string_view;
            using u32string_view = std::u32string_view;
            using wstring_view = std::wstring_view;
        }
    #elif __has_include(<experimental/string_view>)
        #include <experimental/string_view>
        #define IO_HAVE_STRING_VIEW 1
        namespace io_std {
            using std::experimental::basic_string_view;
            using string_view = std::experimental::string_view;
            using u16string_view = std::experimental::u16string_view;
            using u32string_view = std::experimental::u32string_view;
            using wstring_view = std::experimental::wstring_view;
        }
    #endif // (__has_include(<string_view>) || __has_include(<experimental/string_view>))
#endif // __has_include

#if !IO_HAVE_STRING_VIEW
#include "boost_string_view.hpp"
namespace io_std {
using boost::basic_string_view;
using string_view = boost::string_view;
using u16string_view = boost::u16string_view;
using u32string_view = boost::u32string_view;
using wstring_view = boost::wstring_view;
}
#endif // !IO_HAVE_STRING_VIEW

#endif // IO_STD_STRING_VIEW_HPP
