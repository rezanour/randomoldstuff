#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_PLATFORM_H_
#define _GDK_PLATFORM_H_

////////////////////////////////////////////////////////////////////////////////
// abstract platform differences
////////////////////////////////////////////////////////////////////////////////

#ifndef __cplusplus
#error GDK only supports pure C++ at this time
#endif

// core standard headers and numeric typedefs (int32_t, int64_t, etc...)

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <cassert>

using std::size_t;
using std::min;
using std::max;

typedef uint8_t byte_t;

// platform specific includes and defining common fundamental facilities like debug out and atomic int32 operations

#ifdef WIN32

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef GDK_EXPORTDLL
#define GDK_PUBLIC  __declspec(dllexport)
#else
#define GDK_PUBLIC  __declspec(dllimport)
#endif

#define DebugString         OutputDebugStringW
#define AtomicIncrement     InterlockedIncrement
#define AtomicDecrement     InterlockedDecrement

#else // On non-Windows platforms

typedef long HRESULT;

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

// For MAC (and IOS?), atomics are:

#define AtomicIncrement     OSAtomicIncrement32
#define AtomicDecrement     OSAtomicDecrement32

#endif // _WIN32

template <typename Ty>
inline void SafeDelete(_Inout_ Ty*& p)
{
    if (p)
    {
        delete p;
        p = nullptr;
    }
}

template <typename Ty>
inline void SafeDeleteArray(_Inout_ Ty*& p)
{
    if (p)
    {
        delete[] p;
        p = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Calling conventions and interface method declaration/definition helpers

#define GDK_API                 __stdcall
#define GDK_IMETHOD             virtual HRESULT GDK_API
#define GDK_IMETHOD_(type)      virtual type GDK_API
#define GDK_IMETHODIMP          HRESULT GDK_API
#define GDK_IMETHODIMP_(type)   type GDK_API
#define GDK_METHOD              HRESULT GDK_API
#define GDK_METHOD_(type)       type GDK_API

#endif // _GDK_PLATFORM_H_
