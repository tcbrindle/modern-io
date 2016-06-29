
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_POSIX_MMAP_STREAM_READER_HPP
#define IO_POSIX_MMAP_STREAM_READER_HPP

#include <io/posix/mmap_file.hpp>
#include <io/stream_reader.hpp>

namespace io {

template <>
class stream_reader<posix::mmap_file>
        : public rng::view_facade<stream_reader<posix::mmap_file>, rng::unknown>
{
public:
    using stream_type = posix::mmap_file;

    stream_reader() = default;

    stream_reader(stream_type stream)
            : stream_(std::move(stream))
    {}

private:
    friend rng::range_access;

    struct cursor {
        cursor() = default;

        cursor(const stream_reader& sr)
                : ptr_{reinterpret_cast<uint8_t*>(sr.stream_.data())},
                  size_{sr.stream_.size()}
        {}

        unsigned char get() const {
            return *(ptr_ + pos_);
        }

        void set(unsigned char c) const {
            *(ptr_ + pos_) = c;
        }

        void next() {
            ++pos_;
        }

        void prev() {
            --pos_;
        }

        bool equal(const cursor& other) const {
            return pos_ == other.pos_ &&
                    ptr_ == other.ptr_ &&
                    size_ == other.size_;
        }

        void advance(std::ptrdiff_t distance) {
            pos_ += distance;
        }

        std::ptrdiff_t distance_to(const cursor& other) const {
            return other.pos_ - pos_;
        }

        bool done() const {
            return pos_ == size_;
        }

        std::uint8_t* ptr_;
        typename stream_type::size_type size_;
        typename stream_type::offset_type pos_ = 0;
    };

    cursor begin_cursor() const { return cursor{*this}; }

    stream_type stream_{};
};

static_assert(rng::RandomAccessRange<stream_reader<posix::mmap_file>>(), "");

} // end namespace io

#endif // IO_POSIX_MMAP_STREAM_READER_HPP
