
#ifndef IO_STRING_VIEW_STREAM
#define IO_STRING_VIEW_STREAM

#include <io/memory_stream.hpp>
#include <io/io_std/string_view.hpp>

namespace io {

/// A stream which reads from an internal `std::string_view`.
/// The string_view can be accessed using the `str()` method
template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_string_view_stream
        : public detail::memory_stream_impl<basic_string_view_stream<CharT, Traits>, long>
{
public:
    using string_view_type = io_std::basic_string_view<CharT, Traits>;
    using char_type = CharT;
    using size_type = typename string_view_type::size_type;

    /// Default-constructs an empty string_view_stream
    basic_string_view_stream() = default;

    /// Constructs a `string_view_stream` from string_view
    /// The cursor will be positioned at the start of the string
    /// @param string_view String to read
    basic_string_view_stream(string_view_type string_view)
        : str_(string_view)
    {}

    /// Returns the original string_view
    string_view_type str() const noexcept { return str_; }

    const char_type* data() const noexcept { return str_.data(); }

    size_type size() const noexcept { return str_.size(); }

private:
    string_view_type str_;
};

using string_view_stream = basic_string_view_stream<char>;
using u16string_view_stream = basic_string_view_stream<char16_t>;
using u32string_view_stream = basic_string_view_stream<char32_t>;
using wstring_view_stream = basic_string_view_stream<wchar_t>;

} // end namespace io

#endif // IO_STRING_VIEW_STREAM_HPP
