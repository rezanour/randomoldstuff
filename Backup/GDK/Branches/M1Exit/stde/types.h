////////////////////////////////////////////////////////////////////////////////
// Types.h is the root header for the standard extensions library (stde).
// It accounts for platform (OS & compiler) differences, providing a common 
// base for the rest of the headers to build off of.

#ifndef _STDE_TYPES_
#define _STDE_TYPES_ 0x0000 // define with null version to help detect circular include

////////////////////////////////////////////////////////////////////////////////
// Ensure C++ compiler (not C) and also fold build macros into common format

#if !defined(__cplusplus)
#error A C++ compiler is required to use this product. This is not a C compatible API
#endif

#ifndef _DEBUG
#if !defined(NDEBUG)
#define _DEBUG
#endif
#endif

#if defined(NDEBUG) && defined(_DEBUG)
#error Both "NO DEBUG" and "DEBUG" defined at once!
#endif

////////////////////////////////////////////////////////////////////////////////
// Standard headers

#include <cstddef>  // standards compliant size_t and ptrdiff_t
#include <cstdint>  // standard integer types (std::int8_t, etc...)
#include <memory>   // swap() and various smart pointer types

////////////////////////////////////////////////////////////////////////////////
// Ensure that standards compliant versions of the following are used instead 
// of any compiler specific ones. Some implementations do this already, but 
// others don't. Having them repeated here doesn't hurt anything and ensures 
// that they're always used properly.

using std::size_t;
using std::ptrdiff_t;
using std::swap;

////////////////////////////////////////////////////////////////////////////////
// Non-MS C++ compilers don't have __intXX types defined

#if !defined (_MSC_VER)
#define __int8 std::int8_t;
#define __int16 std::int16_t;
#define __int32 std::int32_t;
#define __int64 std::int64_t;
#endif

////////////////////////////////////////////////////////////////////////////////
// Convenient shorthand for common types

typedef unsigned __int8 byte;
typedef unsigned __int8 uint8;
typedef __int8 int8;

typedef unsigned short ushort;
typedef unsigned __int16 uint16;
typedef __int16 int16;

typedef unsigned int uint;
typedef unsigned __int32 uint32;
typedef __int32 int32;

typedef unsigned long ulong;
typedef unsigned __int64 uint64;
typedef __int64 int64;

////////////////////////////////////////////////////////////////////////////////
// Array deleter for use smart pointers to mem allocated with new []

namespace stde
{
    template <typename T>
    struct array_deleter
    {
        void operator()(T* ptr) const
        {
            delete [] ptr;
        }
    };
}

#undef _STDE_TYPES_
#define _STDE_TYPES_ 0x0100 // now change version to proper value

#else // guard already exists, look for circular include

#if _STDE_TYPES_ == 0x0000
#error Circular header inclusion detected!
#endif

#endif


