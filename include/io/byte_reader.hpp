
// Copyright (c) 2017 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MODERN_IO_BYTE_READER_HPP_INCLUDED
#define MODERN_IO_BYTE_READER_HPP_INCLUDED

#include <io/buffer.hpp>
#include <io/read.hpp>
#include <io/traits.hpp>
#include <io/io_std/optional.hpp>

namespace io {

namespace detail {

template <typename Stream, typename = void>
struct has_read_next : std::false_type {};

template <typename Stream>
struct has_read_next<Stream, void_t<
    std::enable_if_t<std::is_same<decltype(std::declval<Stream>().read_next(std::declval<std::error_code&>())),
                                           io_std::optional<unsigned char>>::value>
>> : std::true_type {};

}

template <typename Stream>
struct byte_reader{
    using stream_type = Stream;
    using value_type = unsigned char;
    struct iterator;
    using const_iterator = iterator;

    struct iterator {
        using value_type = typename byte_reader::value_type;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::input_iterator_tag;

        iterator() = default;

        explicit iterator(byte_reader* ptr)
            : range_(ptr)
        {}

        value_type operator*() const
        {
            return range_->val_;
        }

        iterator& operator++()
        {
            range_->next();
            return *this;
        }

        iterator operator++(int)
        {
            iterator temp = *this;
            this->operator++();
            return temp;
        }

        bool operator==(const iterator& other) const
        {
            return done() == other.done() ||
                    range_ == other.range_;
        }

        bool operator!=(const iterator& other) const
        {
            return !(*this == other);
        }

    private:
        bool done() const
        {
            return range_ == nullptr || range_->done_;
        }

        byte_reader* range_ = nullptr;
    };

    byte_reader() = default;

    explicit byte_reader(Stream& stream)
        : stream_(std::addressof(stream))
    {
        next();
    }

    iterator begin() { return iterator{this}; }

    iterator end() { return iterator{}; }

private:

    io_std::optional<unsigned char>
    read_next(std::true_type /*IsBuffered*/, std::error_code& ec)
    {
        return stream_->read_next(ec);
    }

    io_std::optional<unsigned char>
    read_next(std::false_type /*IsBuffered*/, std::error_code& ec)
    {
        char c;
        if (!io::read(*stream_, io::buffer(&c, 1), ec)) {
            return io_std::nullopt;
        }
        return c;
    }


    void next()
    {
        const auto next_val = read_next(detail::has_read_next<stream_type>{}, ec);
        if (next_val) {
            val_ = *next_val;
        } else {
            done_ = true;
        }
    }

    stream_type* stream_ = nullptr;
    std::error_code ec;
    value_type val_{};
    bool done_ = false;
};

template <typename Stream,
          typename = std::enable_if_t<io::is_sync_read_stream_v<Stream>>>
byte_reader<Stream> read(Stream& stream)
{
    return byte_reader<Stream>(stream);
}

}

#endif
