
#ifndef IO_STRING_STREAM
#define IO_STRING_STREAM

#include <io/buffer.hpp>
#include <io/io_concepts.hpp>
#include <io/seek.hpp>

#include <io/io_std/string_view.hpp>

#include <iostream>

namespace io {

/// A debug stream which reads from and writes to an internal `std::string_view`.
/// The string_view can be accessed using the `str()` method
class string_view_stream
{
public:
    /// Offset type for seek operations
    // Since std::string_view::size_type is required to be unsigned, let's just
    // use long.
    using offset_type = long;

    /// Default-constructs a `string_view_stream` with an empty string_view
    string_view_stream() = default;

    /// Constructs a `string_view_stream` with an initial string_view
    /// Positions the stream pointer to the start of the string_view
    /// @param str String to initialise the stream with
    /// @throws std::exception if str could not be initialised
    explicit string_view_stream(io_std::string_view str)
            : str_(std::move(str))
    {}

    /// Get the contained string_view
    const io_std::string_view& str() const noexcept { return str_; }

    /// `SyncReadStream` implementation
    template <typename MutBufSeq,
              CONCEPT_REQUIRES_(MutableBufferSequence<MutBufSeq>())>
    std::size_t read_some(const MutBufSeq& mb)
    {
        std::error_code ec;
        auto bytes_read = read_some(mb, ec);
        if (ec) {
            throw std::system_error{ec};
        }
        return bytes_read;
    }

    /// `SyncReadStream` implementation
    template <typename MutBufSeq,
              CONCEPT_REQUIRES_(MutableBufferSequence<MutBufSeq>())>
    std::size_t read_some(const MutBufSeq& mb, std::error_code& ec)
    {
        ec.clear();

        std::size_t total_buffer_size = io::buffer_size(mb);

        if (total_buffer_size == 0) {
            return 0;
        }

        std::size_t total_bytes_read = 0;
        auto first = io::buffer_sequence_begin(mb);
        auto last = io::buffer_sequence_end(mb);

        while (first != last) {
            std::size_t bytes_read = this->read_some(*first, ec);
            total_bytes_read += bytes_read;

            if (ec) {
                break;
            }

            ++first;
        }

        return total_bytes_read;
    }

    std::size_t read_some(const mutable_buffer& mb, std::error_code& ec)
    {
        ec.clear();

        if (mb.size() == 0) {
            return 0;
        }

        std::size_t bytes_to_read = std::min(mb.size(), str_.size() - pos_);
        char* buf_start = reinterpret_cast<char*>(mb.data());

        char* buf_end = std::copy(str_.data() + pos_,
                                  str_.data() + pos_ + bytes_to_read,
                                  buf_start);

        auto bytes_read = std::distance(buf_start, buf_end);
        pos_ += bytes_read;

        if (typename io_std::string_view::size_type(pos_) == str_.size()) {
            ec = io::make_error_code(io::stream_errc::eof);
        }

        return bytes_read;
    }

    /// SeekableStream implementation
    offset_type seek(offset_type from, seek_mode mode)
    {
        std::error_code ec;
        auto offset = this->seek(from, mode, ec);
        if (ec) {
            throw std::system_error{ec};
        }
        return offset;
    }

    /// SeekableStream implementation
    offset_type seek(offset_type from, seek_mode mode, std::error_code& ec)
    {
        auto old_pos = pos_; // To reset on error

        switch (mode) {
        case seek_mode::current:
            pos_ += from;
            break;
        case seek_mode::start:
            pos_ = from;
            break;
        case seek_mode::end:
            pos_ = str_.size() + from;
            break;
        }

        if (pos_ < 0) {
            pos_ = old_pos;
            ec = std::make_error_code(std::errc::invalid_seek);
        }

        return pos_;
    }

    /// SeekableStream implementation
    offset_type get_position() const noexcept { return pos_; }

private:
    io_std::string_view str_;
    offset_type pos_ = 0;
};

}

#endif // IO_STREAM_STREAM_HPP
