
#ifndef IO_LOCKING_HPP
#define IO_LOCKING_HPP

#include <io/io_concepts.hpp>

#include <mutex>

namespace io {

template <typename Stream> class lockable;

/// Stream adaptor holding a lock on a mutex
/// A `locked_stream` is a stream adaptor which wraps a stream with a
/// lock obtained from a `lockable`. The lock is held for the lifetime of
/// the `locked_stream` -- that is, you *must* allow the `locked_stream` to be
/// destroyed when you have finished using it. The easiest way to do this is to
/// use it as a stack variable.
///
/// You can obtain a locked stream by
///
///  - Calling the lock() member function on a `lockable`
///  - Calling the io::lock() free function, passing a `lockable`
///  - Calling the locked_stream::locked_stream() constructor, passing a
///    `lockable`
///
/// If the `lockable` is currently in use by another thread, the above functions
/// will block until that thread releases its lock by allowing the
/// `locked_stream` to be destroyed.
////
/// A `locked_stream` is a `SyncReadStream` if the underlying stream models this
/// concept. Likewise, it is a `SyncWriteStream` if the underlying stream is
/// a `SyncWriteStream`.
///
/// You can obtain a reference to the underlying stream by calling the
///  locked_stream::base() member function.
template <typename Stream>
class locked_stream
{
public:
    /// The underlying stream type wrapped by this `locked_stream`
    using base_stream_type = Stream;

    /// Construct a `locked_stream` from `lockable`.
    /// This function will block if another thread currently has a
    /// `locked_stream` obtained from the same `lockable`.
    /// @param lockable_ A lockable steam
    explicit locked_stream(lockable<base_stream_type>& lockable_)
        : stream_(lockable_.stream_),
          lock_(*lockable_.mutex_)
    {}

    /// Access the underlying stream
    base_stream_type& base() { return stream_; }

    /// Read some bytes from the underlying stream, if it supports this.
    template <typename MutBufSeq,
              CONCEPT_REQUIRES_(MutableBufferSequence<MutBufSeq>() &&
                                SyncReadStream<Stream>())>
    std::size_t read_some(const MutBufSeq& mb)
    {
        return stream_.read_some(mb);
    }

    /// Read some bytes from the underlying stream, if it supports this
    template <typename MutBufSeq,
            CONCEPT_REQUIRES_(MutableBufferSequence<MutBufSeq>() &&
                              SyncReadStream<Stream>())>
    std::size_t read_some(const MutBufSeq& mb, std::error_code& ec)
    {
        return stream_.read_some(mb, ec);
    }

    /// Write some bytes to the underlying stream, if it supports this
    template <typename ConstBufSeq,
            CONCEPT_REQUIRES_(ConstBufferSequence<ConstBufSeq>() &&
                              SyncWriteStream<Stream>())>
    std::size_t write_some(const ConstBufSeq& mb)
    {
        return stream_.write_some(mb);
    }

    /// Write some bytes to the underlying stream, if it supports this
    template <typename ConstBufSeq,
            CONCEPT_REQUIRES_(ConstBufferSequence<ConstBufSeq>() &&
                              SyncWriteStream<Stream>())>
    std::size_t write_some(const ConstBufSeq& mb, std::error_code& ec)
    {
        return stream_.write_some(mb, ec);
    }

private:
    Stream& stream_;
    std::unique_lock<std::mutex> lock_;
};

/// A lockable wrapper for a stream
///
/// You can think of a lockable as a `pair<Stream, std::mutex>`. The lockable
/// always owns the stream it wraps. You can get access to the underlying stream
/// by calling the `lock()` member function.
///
/// Example usage:
///
/// ```{.cpp}
/// io::lockable<io::file> lockable_file{path};
/// auto stream = lockable_file.lock();
/// stream.read_some(args...)
/// ```
///
template <typename Stream>
class lockable
{
public:
    /// The stream type being wrapped
    using stream_type = Stream;

    /// Constructs the underlying stream from the given arguments
    template <typename... Args,
              CONCEPT_REQUIRES_(std::is_constructible<Stream, Args...>::value)>
    lockable(Args&&... args)
            : stream_(std::forward<Args>(args)...)
    {}

    /// Obtains the lock and returns the locked stream
    ///
    /// This function will block if another thread is currently using the stream
    locked_stream<stream_type> lock()
    {
        return locked_stream<stream_type>{*this};
    }

private:
    friend class locked_stream<stream_type>;

    stream_type stream_;
    std::unique_ptr<std::mutex> mutex_ = std::make_unique<std::mutex>();
};

/// Calls `lockable.lock()`
template <typename Stream>
locked_stream<Stream> lock(lockable<Stream>& lockable)
{
    return lockable.lock();
}

/// A wrapper for `io::read()` which first obtains the stream lock
// TODO: Constrain this with is_callable<> once that is available
template <typename Stream, typename... Args>
decltype(auto) read(lockable<Stream>& stream, Args&&... args)
{
    locked_stream<Stream> locked_{stream};
    return io::read(locked_, std::forward<Args>(args)...);
}

/// A wrapper for `io::write()` which first obtains the stream lock
// TODO: Constrain this with is_callable<> once that is available
template <typename Stream, typename... Args>
decltype(auto) write(lockable<Stream>& stream, Args&&... args)
{
    locked_stream<Stream> locked_{stream};
    return io::write(locked_, std::forward<Args>(args)...);
}

} // end namespace io

#endif // IO_LOCKING_HPP
