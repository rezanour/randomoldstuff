////////////////////////////////////////////////////////////////////////////////
// Platform.h is the root header for the Game Development Kit (GDK).
// It accounts for platform (OS & compiler) differences, providing a common 
// base for the rest of the headers to build off of.

#ifndef _GDK_PLATFORM_
#define _GDK_PLATFORM_ 0x0000 // define with null version to help detect circular include

#include <stde\types.h>

////////////////////////////////////////////////////////////////////////////////
// If MS C++ compiler, we can define to take advantage of a few things...

#if defined(_MSC_VER)
#define INLINE      __forceinline
#else // Non-MS compiler, we define for portability
#define INLINE      inline
#endif

////////////////////////////////////////////////////////////////////////////////
// Ensure we have consistency around HRESULT and SAL 2.0

#ifdef _WIN32

#include <Windows.h> // Gets us HRESULT & SAL

#else // On non-Windows platforms

// Define things that Windows defined for us. We should be avoiding most Windows.h specifics in public API, but there are a few...
typedef long    HRESULT;
typedef void    IStream; // REVIEW: We don't have a proper IStream story for other platforms

#define _In_
#define _In_cap_(x)
#define _In_opt_
#define _In_z_
#define _In_z_opt_

#define _Out_
#define _Out_opt_
#define _Out_z_
#define _Out_z_opt_
#define _Deref_out_
#define _Deref_out_opt_

#define _Inout_
#define _Inout_opt_

#endif

////////////////////////////////////////////////////////////////////////////////
// Calling conventions and interface method declaration/definition helpers

#define GDK_INLINE                  INLINE

#define GDK_API                     __stdcall

#define GDK_METHOD                  virtual HRESULT GDK_API
#define GDK_METHOD_(returnType)     virtual returnType GDK_API

#define GDK_METHODIMP               HRESULT GDK_API
#define GDK_METHODIMP_(returnType)  returnType GDK_API

#undef _GDK_PLATFORM_
#define _GDK_PLATFORM_ 0x0100 // now change version to proper value

#else // guard already exists, look for circular include

#if _GDK_PLATFORM_ == 0x0000
#error Circular header inclusion detected!
#endif

#endif

