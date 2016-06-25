
#ifndef IO_MEMORY_STREAM
#define IO_MEMORY_STREAM

#include <io/buffer.hpp>
#include <io/io_concepts.hpp>
#include <io/seek.hpp>

namespace io {

namespace detail {

template <typename Derived, typename OffsetType>
struct memory_stream_impl
{
    using offset_type = OffsetType;

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

    template <typename MutBufSeq,
            CONCEPT_REQUIRES_(MutableBufferSequence<MutBufSeq>())>
    std::size_t read_some(const MutBufSeq& mb, std::error_code& ec) noexcept
    {
        ec.clear();

        if (pos_ == this->size()) {
            ec = stream_errc::eof;
        }

        auto bytes_copied = io::buffer_copy(mb, this->buffer());
        pos_ += bytes_copied;

        return bytes_copied;
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
    offset_type seek(offset_type from, seek_mode mode, std::error_code& ec) noexcept
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

        return pos_;
    }

    /// SeekableStream implementation
    offset_type get_position() const noexcept { return pos_; }

    const void* data() const noexcept
    {
        return static_cast<const Derived*>(this)->data();
    }

    offset_type size() const noexcept
    {
        return static_cast<const Derived*>(this)->size();
    }

    const_buffer buffer() const noexcept
    {
        return io::buffer(data(), size()) + pos_;
    }

private:
    offset_type pos_ = 0;
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
