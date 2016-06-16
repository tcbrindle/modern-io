
#ifndef IO_POSIX_MMAP_STREAM_READER_HPP
#define IO_POSIX_MMAP_STREAM_READER_HPP

#include <io/posix/mmap_file.hpp>

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

        cursor(const stream_reader& sr) : buf_{sr.stream_.data()} {}

        unsigned char get() const {
            return *(static_cast<const unsigned char*>(buf_.data()) + pos_);
        }

        void set(unsigned char c) const {
            *(static_cast<unsigned char*>(buf_.data()) + pos_) = c;
        }

        void next() {
            ++pos_;
        }

        void prev() {
            --pos_;
        }

        bool equal(const cursor& other) const {
            return pos_ == other.pos_ &&
                    buf_.data() == other.buf_.data() &&
                    buf_.size() == other.buf_.size();
        }

        void advance(std::ptrdiff_t distance) {
            pos_ += distance;
        }

        std::ptrdiff_t distance_to(const cursor& other) const {
            return other.pos_ - pos_;
        }

        bool done() const {
            return pos_ == buf_.size();
        }

        mutable_buffer buf_{};
        std::size_t pos_ = 0;
    };

    cursor begin_cursor() const { return cursor{*this}; }

    stream_type stream_;
};

static_assert(rng::RandomAccessRange<stream_reader<posix::mmap_file>>(), "");

} // end namespace io

#endif // IO_POSIX_MMAP_STREAM_READER_HPP
