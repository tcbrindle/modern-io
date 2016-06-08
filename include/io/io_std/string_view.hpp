
#ifndef IO_STD_STRING_VIEW_HPP
#define IO_STD_STRING_VIEW_HPP

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


#endif // IO_STD_STRING_VIEW_HPP
