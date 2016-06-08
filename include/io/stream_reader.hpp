
#ifndef IO_STREAM_READER_HPP
#define IO_STREAM_READER_HPP

#include <io/io_concepts.hpp>
#include <io/buffer.hpp>
#include <io/types.hpp>

#include <range/v3/view_facade.hpp>

#include <array>
#include <cstdio> // For BUFSIZ


namespace io {

namespace rng = ranges::v3;

template <typename Stream, std::size_t BufferSize = BUFSIZ,
          CONCEPT_REQUIRES_(SyncReadStream<Stream>())>
class stream_reader
    : public rng::view_facade<stream_reader<Stream>, rng::unknown>
{
public:
    using stream_type = Stream;
    using buffer_type = std::array<byte, BufferSize>;

    stream_reader() = default;

    stream_reader(stream_type stream)
            : stream_(std::move(stream)),
              done_{false}
    {
        fill_buffer();
    }

    explicit operator bool() { return !done_; }

private:
    friend rng::range_access;

    struct cursor {
        cursor() = default;

        cursor(stream_reader& sr) : sr_{&sr} {}

        byte get() const
        {
            return *sr_->it_;
        }

        void next()
        {
            return sr_->next();
        }

        bool done() const {
            return sr_->done();
        }

        stream_reader* sr_ = nullptr;
    };

    cursor begin_cursor() { return cursor{*this}; }

    void next()
    {
        if (++it_ == rng::end(buf_) && !done_) {
            fill_buffer();
        }
    }

    bool done() const
    {
        return (it_ == last_);
    }

    void fill_buffer()
    {
        std::error_code ec{};
        auto bytes_read = io::read(stream_, io::buffer(buf_), ec);
        it_ = rng::begin(buf_);
        last_ = it_ + bytes_read;

        if (ec) {
            if (ec.value() == static_cast<int>(stream_errc::eof)) {
                done_ = true;
            } else {
                throw std::system_error(ec);
            }
        }
    }


    stream_type stream_{};
    buffer_type buf_{{}};
    typename buffer_type::const_iterator it_{};
    typename buffer_type::const_iterator last_{};
    bool done_ = true;
};

template <typename Stream,
        CONCEPT_REQUIRES_(SyncReadStream<Stream>())>
stream_reader<Stream> read(Stream&& stream)
{
    return stream_reader<Stream>{std::forward<Stream>(stream)};
}

}

#endif // IO_STREAM_READER_HPP
