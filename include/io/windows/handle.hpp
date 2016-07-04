

#ifndef MODERN_IO_HANDLE_HPP
#define MODERN_IO_HANDLE_HPP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cassert>

namespace io {
    namespace windows {

        /// Move-only RAII wrapper around a Windows HANDLE
        struct handle_t
        {
            handle_t() = default;

            handle_t(HANDLE h, bool transfer_ownership = true)
                : handle_(h),
                  delete_(transfer_ownership)
            {}

            handle_t(handle_t&& other)
                    : handle_(other.handle_),
                      delete_(other.delete_)
            {
                other.handle_ = nullptr;
                other.delete_ = false;
            }

            handle_t& operator=(handle_t&& other)
            {
                if (&other != this) {
                    std::swap(handle_, other.handle_);
                    std::swap(delete_, other.delete_);
                }
                return *this;
            }

            ~handle_t()
            {
                if (delete_) {
                    if (::CloseHandle(handle_) == 0) {
                        assert(true);
                    }
                }
            }

            void close(std::error_code& ec)
            {
                if (::CloseHandle(handle_) == 0) {
                    ec.assign(::GetLastError(), std::system_category());
                }
            }

            HANDLE get() const { return handle_; }

            explicit operator bool() const { return handle_ != nullptr; }

        private:
            HANDLE handle_ = nullptr;
            bool delete_ = false;
        };

    }
}

#endif //MODERN_IO_HANDLE_HPP
