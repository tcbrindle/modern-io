
// Copyright (c) 2017 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MODERN_IO_BYTE_RANGE_HPP_INCLUDED
#define MODERN_IO_BYTE_RANGE_HPP_INCLUDED

#include <io/buffer.hpp>
#include <io/read.hpp>

namespace io {

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
    void next()
    {
        io::mutable_buffer buf(&val_, 1);
        io::read(*stream_, buf, ec);
        if (ec) {
            if (ec == io::stream_errc::eof) {
                done_ = true;
            }
            else  {
                throw std::system_error(ec);
            }
        }
    }

    stream_type* stream_ = nullptr;
    std::error_code ec;
    value_type val_{};
    bool done_ = false;
};



}

#endif
