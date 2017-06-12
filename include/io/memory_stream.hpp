
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_MEMORY_STREAM
#define IO_MEMORY_STREAM

#include <io/buffer.hpp>
#include <io/stream_position.hpp>
#include <io/traits.hpp>
#include <io/seek.hpp>
#include <io/io_std/optional.hpp>

namespace io {

namespace detail {

template <typename Derived, typename OffsetType>
struct memory_stream_impl
{
    using offset_type = OffsetType;
    using position_type = io::stream_position<offset_type>;
    using size_type = offset_type;
    using value_type = unsigned char;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = const_pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    template <typename MutBufSeq>
    std::size_t read_some(const MutBufSeq& mb)
    {
        std::error_code ec;
        auto bytes_read = read_some(mb, ec);
        if (ec) {
            throw std::system_error{ec};
        }
        return bytes_read;
    }

    template <typename MutBufSeq>
    std::size_t read_some(const MutBufSeq& mb, std::error_code& ec) noexcept
    {
        static_assert(io::is_mutable_buffer_sequence_v<MutBufSeq>,
                      "Argument passed to read_some() is not a MutableBufferSequence");

        ec.clear();

        if (pos_ == this->size()) {
            ec = stream_errc::eof;
        }

        auto bytes_copied = io::buffer_copy(mb, this->buffer());
        pos_ += bytes_copied;

        return bytes_copied;
    }

    unsigned char read_next()
    {
        std::error_code ec;
        const auto opt = read_next(ec);
        if (opt) {
            return *opt;
        } else {
            throw std::system_error{ec};
        }
    }

    io_std::optional<unsigned char> read_next(std::error_code& ec)
    {
        if (pos_ == size()) {
            ec = io::stream_errc::eof;
            return io_std::nullopt;
        } else {
            return static_cast<const_pointer>(data())[pos_++];
        }
    }

    unsigned char peek_next()
    {
        std::error_code ec;
        const auto opt = peek_next(ec);
        if (opt) {
            return *opt;
        } else {
            throw std::system_error{ec};
        }
    }

    io_std::optional<unsigned char> peek_next(std::error_code& ec)
    {
        if (pos_ == size()) {
            ec = io::stream_errc::eof;
            return io_std::nullopt;
        } else {
            return static_cast<const unsigned char*>(data())[pos_];
        }
    }

    /// SeekableStream implementation
    position_type seek(offset_type from, seek_mode mode)
    {
        std::error_code ec;
        auto pos = this->seek(from, mode, ec);
        if (ec) {
            throw std::system_error{ec};
        }
        return pos;
    }

    /// SeekableStream implementation
    position_type seek(offset_type from, seek_mode mode, std::error_code& ec) noexcept
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
            pos_ = this->size() + from;
            break;
        }

        if (pos_ < 0 || pos_ > this->size()) {
            pos_ = old_pos;
            ec = std::make_error_code(std::errc::invalid_seek);
        }

        return position_type{pos_};
    }

    /// SeekableStream implementation
    position_type get_position() const noexcept { return position_type{pos_}; }

    const void* data() const noexcept
    {
        return static_cast<const Derived*>(this)->data();
    }

    size_type size() const noexcept
    {
        return static_cast<const Derived*>(this)->size();
    }

    const_buffer buffer() const noexcept
    {
        return io::buffer(data(), size()) + pos_;
    }

    // Iterator support
    iterator begin() { return static_cast<iterator>(this->data()); }
    const_iterator begin() const { return static_cast<const_iterator>(this->data()); }
    const_iterator cbegin() const { return begin(); }

    iterator end() { return begin() + this->size(); }
    const_iterator end() const { return begin() + this->size(); }
    const_iterator cend() const { return end(); }

    reverse_iterator rbegin() { return reverse_iterator{end()}; }
    const_reverse_iterator rbegin() const { return const_reverse_iterator{end()}; }
    const_reverse_iterator crbegin() const { return rbegin(); }

    reverse_iterator rend() { return reverse_iterator{begin()}; }
    const_reverse_iterator rend() const { return const_reverse_iterator{begin()}; }
    const_reverse_iterator crend() const { return rend(); }

private:
    offset_type pos_{};
};

} // end namespace detail

/// A stream which reads from a contiguous area of memory
class memory_stream : public detail::memory_stream_impl<memory_stream, std::ptrdiff_t>
{
public:
    /// Default-constructs an empty `memory_stream`
    memory_stream() = default;

    /// Constructs a `memory_stream` from a pointer and a size
    /// @ptr A pointer to the start of a memory region
    /// @size_bytes The size of the memory region, in bytes
    memory_stream(const void* ptr, std::size_t size_bytes) noexcept
        : start_(reinterpret_cast<const std::uint8_t*>(ptr)),
          size_(size_bytes)
    {}

    /// Return a pointer to the start of the data area
    const std::uint8_t* data() const noexcept { return start_; }

    /// Return the total size of the data area, in bytes
    std::size_t size() const noexcept { return size_; }

private:
    const std::uint8_t* start_ = nullptr;
    std::size_t size_ = 0;
};

} // end namespace io

#endif // IO_MEMORY_STREAM_HPP
