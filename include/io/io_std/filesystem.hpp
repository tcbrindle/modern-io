
#ifndef IO_STD_FILESYSTEM_HPP
#define IO_STD_FILESYSTEM_HPP

#ifdef __has_include
#if __has_include(<filesystem>)
        #include <filesystem>
        #define IO_HAVE_STD_FILESYSTEM 1
        namespace io_std {
            namespace filesystem {
                using std::filesystem::path;
                using std::filesystem::perms;
            }
        }
    #elif __has_include(<experimental/filesystem>)
        #include <experimental/filesystem>
        #define IO_HAVE_STD_FILESYSTEM 1
        namespace io_std {
            namespace filesystem {
                using std::experimental::filesystem::path;
                using std::experimental::filesystem::perms;
            }
        }
    #endif // (__has_include(<string_view>) || __has_include(<experimental/string_view>))
#endif // __has_include

#if !IO_HAVE_STD_FILESYSTEM

#include <string>

namespace io_std {
namespace filesystem {

enum class perms {
    none = 0,
    owner_read = 0400,
    owner_write = 0200,
    owner_exec = 0100,
    owner_all = 0700,
    group_read = 040,
    group_write = 020,
    group_exec = 010,
    group_all = 070,
    others_read = 04,
    others_write = 02,
    others_exec = 01,
    others_all = 07,
    all = 0777,
    set_uid = 04000,
    set_gid = 02000,
    sticky_bit = 01000,
    mask = 07777,

    unknown = 0xFFFF,
    add_perms = 0x10000,
    remove_perms = 0x20000,
    resolve_symlinks = 0x40000
};

constexpr perms operator&(perms lhs, perms rhs)
{
    using u = std::underlying_type_t<perms>;
    return static_cast<perms>(static_cast<u>(lhs) & static_cast<u>(rhs));
}

constexpr perms operator|(perms lhs, perms rhs)
{
    using u = std::underlying_type_t<perms>;
    return static_cast<perms>(static_cast<u>(lhs) | static_cast<u>(rhs));
}

constexpr perms operator^(perms lhs, perms rhs)
{
    using u = std::underlying_type_t<perms>;
    return static_cast<perms>(static_cast<u>(lhs) ^ static_cast<u>(rhs));
}

constexpr perms operator~(perms p)
{
    using u = std::underlying_type_t<perms>;
    return static_cast<perms>(~static_cast<u>(p));
}

constexpr perms& operator&=(perms& lhs, perms rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

constexpr perms& operator|=(perms& lhs, perms rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

using path = std::string;

} // end namespace filesystem
} // end namespace perms
#endif // !IO_HAVE_STD_FILESYSTEM

#endif // IO_STD_FILESYSTEM_HPP
