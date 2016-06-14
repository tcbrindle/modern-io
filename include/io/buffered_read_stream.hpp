
// Uses code from ASIO

#ifndef IO_BUFFERED_READ_STREAM_HPP
#define IO_BUFFERED_READ_STREAM_HPP

#include <io/detail/owned_vector_buffer.hpp>
#include <io/io_concepts.hpp>
#include <io/read.hpp>
#include <io/types.hpp>

#include <cstdio> // for BUFSIZ

namespace io {

template <typename Stream, typename Allocator = std::allocator<io::byte>>
class buffered_read_stream
{
public:
    using allocator_type = Allocator;
    using base_stream_type = Stream;
    using buffer_type = detail::owned_vector_buffer<io::byte, allocator_type>;

    static constexpr std::size_t default_buffer_size = BUFSIZ;

    /// Default constructor
    // TODO: enable_if<is_default_constructible<Stream>::value>
    buffered_read_stream() = default;

    /// Constructor from base stream
    explicit buffered_read_stream(base_stream_type base)
        : base_(std::move(base))
    {}

    /// Constructor from base stream with max size
    buffered_read_stream(std::size_t max_buffer_size,
                         base_stream_type base)
        : base_(std::move(base)),
          buf_(max_buffer_size)
    {}

    /// Constructor from base stream with max size and allocator
    buffered_read_stream(std::size_t max_buffer_size,
                         const allocator_type& allocator,
                         base_stream_type base)
        : base_(std::move(base)),
          buf_(allocator, max_buffer_size)
    {}

    /// Forwarding constructor for base stream
    template <typename... Args,
              CONCEPT_REQUIRES_(std::is_constructible<base_stream_type, Args...>::value)>
    buffered_read_stream(Args&&... args)
            : base_(std::forward<Args>(args)...)
    {}

    template <typename... Args,
            CONCEPT_REQUIRES_(std::is_constructible<base_stream_type, Args...>::value)>
    buffered_read_stream(std::size_t max_buffer_size, Args&&... args)
            : base_(std::forward<Args>(args)...),
              buf_(max_buffer_size)
    {}

    template <typename... Args,
            CONCEPT_REQUIRES_(std::is_constructible<base_stream_type, Args...>::value)>
    buffered_read_stream(std::size_t max_buffer_size,
                         const allocator_type& allocator,
                         Args&&... args)
            : base_(std::forward<Args>(args)...),
              buf_(allocator, max_buffer_size)
    {}

    allocator_type get_allocator() const { return buf_.get_allocator(); }

    base_stream_type& next_layer() noexcept { return base_; }
    const base_stream_type& next_layer() const noexcept { return base_; }

    template <typename MutBufSeq,
              CONCEPT_REQUIRES_(MutableBufferSequence<MutBufSeq>())>
    std::size_t read_some(const MutBufSeq& mb)
    {
        std::error_code ec;
        std::size_t bytes_read = this->read_some(mb, ec);
        if (ec) {
            throw std::system_error{ec};
        }
        return bytes_read;
    }

    template <typename MutBufSeq,
            CONCEPT_REQUIRES_(MutableBufferSequence<MutBufSeq>())>
    std::size_t read_some(const MutBufSeq& mb, std::error_code& ec)
    {
        ec.clear();

        if (io::buffer_size(mb) == 0) {
            return 0;
        }

        if (buf_.size() == 0) {
            this->fill(ec);
        }

        return this->copy(mb);
    }

    template <typename ConstBufSeq,
            CONCEPT_REQUIRES_(ConstBufferSequence<ConstBufSeq>() &&
                              SyncWriteStream<base_stream_type>())>
    std::size_t write_some(const ConstBufSeq& mb)
    {
        return base_.write_some(mb);
    }

    template <typename ConstBufSeq,
            CONCEPT_REQUIRES_(ConstBufferSequence<ConstBufSeq>() &&
                              SyncWriteStream<base_stream_type>())>
    std::size_t write_some(const ConstBufSeq& mb, std::error_code& ec)
    {
        return base_.write_some(mb, ec);
    }

    std::size_t fill()
    {
        std::error_code ec;
        std::size_t bytes_read = this->fill(ec);
        if (ec) {
            throw std::system_error{ec};
        }
        return bytes_read;
    }

    std::size_t fill(std::error_code& ec)
    {
        buf_.clear();
        return io::read(base_, buf_, ec);
    }


private:
    // Copies from our internal buffer to the external buffer
    template <typename MutBufSeq>
    std::size_t copy(const MutBufSeq& mb)
    {
        auto bytes_copied = io::buffer_copy(mb, buf_.data(), buf_.size());
        buf_.consume(bytes_copied);
        return bytes_copied;
    }


    base_stream_type base_{};
    buffer_type buf_{allocator_type{}, default_buffer_size};
};

} // end namespace io

#endif // IO_BUFFERED_READ_STREAM_HPP
