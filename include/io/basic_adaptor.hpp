
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_BASIC_ADAPTOR_HPP
#define IO_BASIC_ADAPTOR_HPP

#include <io/traits.hpp>
#include <io/seek.hpp>

#include <type_traits>

namespace io {

/// Base class for stream adaptors.
/// It acts as an identity adaptor, forwarding read_some() and write_some()
/// calls where they exist on the wrapped stream.
///
/// Note that this class is strictly a base class; you should not use it
/// directly, nor cast any adaptor type to it (it has a protected destructor to
/// discourage you).
template <typename Stream>
class basic_adaptor
{
public:
    /// Type of the stream we are wrapping
    using stream_type = Stream;
    /// Also the type of the stream we are wrapping
    using next_layer_type = stream_type;

    /// Provides access to the underlying wrapped stream
    next_layer_type& next_layer() noexcept { return stream_; }
    /// Provides access to the underlying wrapped stream
    const next_layer_type& next_layer() const noexcept { return stream_; }

    /// Default-constructs an adaptor
    /// This will in turn default-construct the underlying stream
    basic_adaptor() = default;

    basic_adaptor(basic_adaptor&&) = default;
    basic_adaptor& operator=(basic_adaptor&&) = default;

    /// Constructs an adaptor around an already-existing `Stream`.
    /// Note that adaptors always take ownership of their underlying streams,
    /// and since streams are not copyable, you'll probably need to pass an
    /// rvalue in here via std::move().
    basic_adaptor(stream_type stream)
            : stream_(std::move(stream))
    {}

    /// Constructs the underlying stream from the given arguments.
    /// Simply forwards to the underlying constructor.
    template <typename... Args,
              typename = std::enable_if_t<std::is_constructible<stream_type, Args...>::value>>
    basic_adaptor(Args&&... args)
            : stream_(std::forward<Args>(args)...)
    {}

    /// Read some bytes from the stream, if the underlying stream support this.
    /// Forwards to stream_type::read_some(), if that exists.
    template <typename MutBufSeq, typename S = stream_type,
              typename = std::enable_if_t<is_sync_read_stream_v<S>>>
    decltype(auto) read_some(const MutBufSeq& mb)
    {
        return stream_.read_some(mb);
    }

    /// Read some bytes from the stream, if the underlying stream support this.
    /// Forwards to stream_type::read_some(), if that exists.
    template <typename MutBufSeq, typename S = stream_type,
              typename = std::enable_if_t<is_sync_read_stream_v<S>>>
    decltype(auto) read_some(const MutBufSeq& mb, std::error_code& ec)
    {
        return stream_.read_some(mb, ec);
    }

    /// Write some bytes to the stream, if the underlying stream support this.
    /// Forwards to stream_type::write_some(), if that exists.
    template <typename ConstBufSeq, typename S = stream_type,
              typename = std::enable_if_t<is_sync_write_stream_v<S>>>
    decltype(auto) write_some(const ConstBufSeq& cb)
    {
        return stream_.write_some(cb);
    }

    /// Write some bytes to the stream, if the underlying stream support this.
    /// Forwards to stream_type::write_some(), if that exists.
    template <typename ConstBufSeq, typename S = stream_type,
              typename = std::enable_if_t<is_sync_write_stream_v<S>>>
    decltype(auto) write_some(const ConstBufSeq& cb, std::error_code& ec)
    {
        return stream_.write_some(cb, ec);
    }

    /// Performs a seek, if the underlying stream supports this.
    /// Forwards to stream_type::seek(), if that exists
    template <typename OffsetType, typename S = stream_type,
              typename = std::enable_if_t<is_seekable_stream_v<S>>>
    decltype(auto) seek(OffsetType distance, seek_mode from)
    {
        return stream_.seek(distance, from);
    }

    /// Performs a seek, if the underlying stream supports this.
    /// Forwards to stream_type::seek(), if that exists
    template <typename OffsetType, typename S = stream_type,
              typename = std::enable_if_t<is_seekable_stream_v<S>>>
    decltype(auto) seek(OffsetType distance, seek_mode from, std::error_code& ec)
    {
        return stream_.seek(distance, from, ec);
    }

protected:
    ~basic_adaptor() = default;

private:
    stream_type stream_{};
};

} // end namespace io

#endif // IO_BASIC_ADAPTOR_HPP
