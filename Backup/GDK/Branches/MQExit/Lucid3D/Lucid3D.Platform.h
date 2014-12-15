// base header for all of Lucid3D usage. This may be included standalone.
// It's also included by almost every Lucid component.

#ifndef _L3DPLATFORM_
#define _L3DPLATFORM_ 0x0000 // define with null version to help detect circular include

// include standard types
#include <cstdint>
#include <cstddef>

// the standard doesn't define a byte typedef
typedef uint8_t byte_t;

// standardize inline keywords
#if defined(_MSC_VER)

#define L3DINLINE       inline
#define L3DFORCEINLINE  __forceinline

#else // Non-MS compiler, we define for portability

#define L3DINLINE       inline
#define L3DFORCEINLINE  inline

#endif // defined (_MSC_VER)

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x) (x)
#endif

// ensure we have SAL 2.0 when available. When it's not, then 
// we define the macros so that our code still compiles
#ifdef _WIN32

#include <sal.h>

// disable warning about non-standard nameless struct/union
#pragma warning(disable: 4201)

#else // On non-Windows platforms

#define _In_
#define _In_count_(x)
#define _In_opt_
#define _In_z_
#define _In_z_opt_

#define _Out_
#define _Out_cap_(x)
#define _Out_opt_
#define _Out_z_
#define _Out_z_opt_
#define _Deref_out_
#define _Deref_out_opt_

#define _Inout_
#define _Inout_cap_(x)
#define _Inout_opt_

#endif // _WIN32

#undef _L3DPLATFORM_
#define _L3DPLATFORM_ 0x0100 // now change version to proper value

#else // guard already exists, look for circular include

#if _L3DPLATFORM_ == 0x0000
#error Circular header inclusion detected!
#endif

#endif
