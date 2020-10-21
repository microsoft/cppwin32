#pragma once

#include <array>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <type_traits>

#ifdef _DEBUG

#define WIN32_ASSERT _ASSERTE
#define WIN32_VERIFY WIN32_ASSERT
#define WIN32_VERIFY_(result, expression) WIN32_ASSERT(result == expression)

#else

#define WIN32_ASSERT(expression) ((void)0)
#define WIN32_VERIFY(expression) (void)(expression)
#define WIN32_VERIFY_(result, expression) (void)(expression)

#endif

#ifndef WIN32_EXPORT
#define WIN32_EXPORT
#endif

#ifdef __IUnknown_INTERFACE_DEFINED__
#define WIN32_IMPL_IUNKNOWN_DEFINED
#endif

#ifdef _M_HYBRID
#define WIN32_IMPL_LINK(function, count) __pragma(comment(linker, "/alternatename:#WIN32_IMPL_" #function "@" #count "=#" #function "@" #count))
#elif _M_ARM64EC
#define WIN32_IMPL_LINK(function, count) __pragma(comment(linker, "/alternatename:#WIN32_IMPL_" #function "=#" #function))
#elif _M_IX86
#define WIN32_IMPL_LINK(function, count) __pragma(comment(linker, "/alternatename:_WIN32_IMPL_" #function "@" #count "=_" #function "@" #count))
#else
#define WIN32_IMPL_LINK(function, count) __pragma(comment(linker, "/alternatename:WIN32_IMPL_" #function "=" #function))
#endif

WIN32_EXPORT namespace win32
{
    struct guid
    {
        uint32_t Data1;
        uint16_t Data2;
        uint16_t Data3;
        uint8_t  Data4[8];

        guid() noexcept = default;

        constexpr guid(uint32_t const Data1, uint16_t const Data2, uint16_t const Data3, std::array<uint8_t, 8> const& Data4) noexcept :
            Data1(Data1),
            Data2(Data2),
            Data3(Data3),
            Data4{ Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7] }
        {
        }

#ifdef WIN32_IMPL_IUNKNOWN_DEFINED

        constexpr guid(GUID const& value) noexcept :
            Data1(value.Data1),
            Data2(value.Data2),
            Data3(value.Data3),
            Data4{ value.Data4[0], value.Data4[1], value.Data4[2], value.Data4[3], value.Data4[4], value.Data4[5], value.Data4[6], value.Data4[7] }
        {

        }

        operator GUID const& () const noexcept
        {
            return reinterpret_cast<GUID const&>(*this);
        }

#endif
    };

    inline bool operator==(guid const& left, guid const& right) noexcept
    {
        return !memcmp(&left, &right, sizeof(left));
    }

    inline bool operator!=(guid const& left, guid const& right) noexcept
    {
        return !(left == right);
    }

    inline bool operator<(guid const& left, guid const& right) noexcept
    {
        return memcmp(&left, &right, sizeof(left)) < 0;
    }
}

namespace win32::_impl_
{
    template <typename T>
    constexpr auto to_underlying_type(T const value) noexcept
    {
        return static_cast<std::underlying_type_t<T>>(value);
    }
}
