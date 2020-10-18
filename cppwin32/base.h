#pragma once

#ifdef _M_HYBRID
#define WIN32_IMPL_LINK(function, count) __pragma(comment(linker, "/alternatename:#WIN32_IMPL_" #function "@" #count "=#" #function "@" #count))
#elif _M_ARM64EC
#define WIN32_IMPL_LINK(function, count) __pragma(comment(linker, "/alternatename:#WIN32_IMPL_" #function "=#" #function))
#elif _M_IX86
#define WIN32_IMPL_LINK(function, count) __pragma(comment(linker, "/alternatename:_WIN32_IMPL_" #function "@" #count "=_" #function "@" #count))
#else
#define WIN32_IMPL_LINK(function, count) __pragma(comment(linker, "/alternatename:WIN32_IMPL_" #function "=" #function))
#endif
