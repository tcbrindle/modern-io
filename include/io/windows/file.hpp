


#ifndef MODERN_IO_FILE_HPP
#define MODERN_IO_FILE_HPP

#include <io/open_mode.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace io {
    namespace windows {
        namespace detail {
            constexpr DWORD seek_mode_to_windows_mode(seek_mode m)
            {
                switch (m) {
                    case seek_mode::start: return FILE_BEGIN;
                    case seek_mode::current: return FILE_CURRENT;
                    case seek_mode::end: return FILE_END;
                }
                return -1;
            }
        }


        class file {
        public:
            using offset_type = __int64;
            using native_handle_type = HANDLE;

            file() = default;

            file(const io_std::filesystem::path& path,
                 open_mode mode,
                 io_std::filesystem::perms create_perms = default_creation_perms)
            {
                this->open(path, mode, create_perms);
            }

            void open(const io_std::filesystem::path& path,
                      open_mode mode,
                      io_std::filesystem::perms create_perms = default_creation_perms)
            {
                std::error_code ec;
                this->open(path, mode, create_perms, ec);
                if (ec) {
                    throw std::system_error{ec};
                }
            }


            void open(const io_std::filesystem::path& path,
                      open_mode mode,
                      io_std::filesystem::perms create_perms,
                      std::error_code& ec) noexcept
            {

            }

            void close()
            {
                std::error_code ec;
                this->close(ec);
                if (ec) {
                    throw std::system_error{ec};
                }
            }

            void close(std::error_code& ec) noexcept
            {
                if (!CloseHandle(handle_)) {
                    ec.assign(GetLastError(), std::system_category());
                };
            }

            native_handle_type native_handle() const noexcept
            {
                return handle_;
            }

            // SyncReadStream implementation

            template <typename MutBufSeq,
                    CONCEPT_REQUIRES_(MutableBufferSequence<MutBufSeq>())>
            std::size_t read_some(const MutBufSeq& mb)
            {
                std::error_code ec;
                auto sz = this->read_some(mb, ec);
                if (ec) {
                    throw std::system_error{ec};
                }
                return sz;
            }

            template <typename MutBufSeq,
                    CONCEPT_REQUIRES_(MutableBufferSequence<MutBufSeq>())>
            std::size_t read_some(const MutBufSeq& mb, std::error_code& ec) noexcept;

            std::size_t read_some(const io::mutable_buffer& mb, std::error_code& ec);

            // SyncWriteStream implementation

            template <typename ConstBufSeq,
                    CONCEPT_REQUIRES_(ConstBufferSequence<ConstBufSeq>())>
            std::size_t write_some(ConstBufSeq& cb)
            {
                std::error_code ec;
                auto sz = write_some(cb, ec);

                if (ec) {
                    throw std::system_error{ec};
                }

                return sz;
            }

            template <typename ConstBufSeq,
                    CONCEPT_REQUIRES_(ConstBufferSequence<ConstBufSeq>())>
            std::size_t write_some(ConstBufSeq& cb, std::error_code& ec) noexcept;

            std::size_t write_some(const io::const_buffer& cb, std::error_code& ec);

            offset_type seek(offset_type offset, seek_mode from)
            {
                std::error_code ec;
                auto o = this->seek(offset, from, ec);
                if (ec) {
                    throw std::system_error(ec);
                }
                return o;
            }

            offset_type
            seek(offset_type offset, seek_mode from, std::error_code& ec) noexcept
            {
                LARGE_INTEGER l_offset{};
                l_offset.QuadPart = offset;
                LARGE_INTEGER new_pos{};
                bool success = SetFilePointerEx(handle_, l_offset, &new_pos, detail::seek_mode_to_windows_mode(from));
                if (!success) {
                    ec.assign(GetLastError(), std::system_category());
                }
                return new_pos.QuadPart;
            }

            void sync(std::error_code& ec) noexcept;

            void sync()
            {
                std::error_code ec{};
                this->sync(ec);
                if (ec) {
                    throw std::system_error{ec};
                }
            }

        private:
            HANDLE handle_ = nullptr;
        };

        static_assert(SeekableStream<windows::file>(),
                      "windows::file does not meet the SeekableStream requirements");

    }
}

#endif //MODERN_IO_FILE_HPP
