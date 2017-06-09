
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_STRING_STREAM
#define IO_STRING_STREAM

#include <io/buffer.hpp>
#include <io/traits.hpp>
#include <io/memory_stream.hpp>
#include <io/seek.hpp>

#include <iostream>

namespace io {

/// A debug stream which reads from and writes to an internal `std::string`.
/// The string can be accessed using the `str()` method
template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_string_stream
    : public detail::memory_stream_impl<basic_string_stream<CharT, Traits>, long>
{
public:
    using string_type = std::basic_string<CharT, Traits>;
    using char_type = CharT;
    using size_type = typename string_type::size_type;

    /// Default-constructs a `string_stream` with an empty string
    basic_string_stream() = default;

    /// Constructs a `string_stream` with an initial string
    /// Positions the stream pointer to the start of the string
    /// @param str String to initialise the stream with
    /// @throws std::exception if str could not be initialised
    explicit basic_string_stream(string_type str)
            : str_(std::move(str))
    {}

    /// Get the contained string
    const string_type& str() const noexcept { return str_; }


    /// SyncWriteStream implementation
    template <typename ConstBufSeq>
    std::size_t write_some(const ConstBufSeq& cb)
    {
        std::error_code ec;
        auto bytes_written = this->write_some(cb, ec);
        if (ec) {
            throw std::system_error{ec};
        }
        return bytes_written;
    }

    /// SyncWriteStream implementation
    template <typename ConstBufSeq>
    std::size_t write_some(const ConstBufSeq& cb, std::error_code& ec)
    {
        static_assert(is_const_buffer_sequence_v<ConstBufSeq>,
                      "Argument passed to write_some() is not a ConstBufferSequence");

        ec.clear();

        if (io::buffer_size(cb) == 0) {
            return 0;
        }

        std::size_t total_bytes_written = 0;
        auto first = io::buffer_sequence_begin(cb);
        auto last = io::buffer_sequence_end(cb);

        while (first != last) {
            str_.insert(this->get_position(),
                        reinterpret_cast<const char_type*>(first->data()),
                        first->size()/sizeof(char_type));
            total_bytes_written += first->size();
            this->seek(first->size(), seek_mode::current, ec);
            ++first;
        }

        return total_bytes_written;
    }

    const char_type* data() const noexcept { return str_.data(); }

    size_type size() const noexcept { return str_.size(); }

private:
    string_type str_;
};

using string_stream = basic_string_stream<char>;
using u16string_stream = basic_string_stream<char16_t>;
using u32string_stream = basic_string_stream<char32_t>;
using wstring_stream = basic_string_stream<wchar_t>;

}

#endif // IO_STREAM_STREAM_HPP
