#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_PLATFORM_H_
#define _GDK_PLATFORM_H_

////////////////////////////////////////////////////////////////////////////////
// Ensure we have consistency around HRESULT and SAL 2.0

#ifdef _WIN32

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // Gets us HRESULT & SAL

#else // On non-Windows platforms

// Define things that Windows defined for us. We should be avoiding most Windows.h specifics in public API, but there are a few...
typedef long    HRESULT;

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
// standard types

#ifdef __cplusplus

#include <cstddef>
#include <cstdint>
#include <numeric>

using std::size_t;
using std::min;
using std::max;

#else

typedef char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef int int32_t;
typedef unsigned int uint32_t;

typedef long long int64_t;
typedef unsigned long long uint64_t;

#define nullptr 0;

#endif // __cplusplus

typedef uint8_t byte_t;

////////////////////////////////////////////////////////////////////////////////
// standard headers

#include <malloc.h>

#ifdef __cplusplus

#include <new>
#include <cassert>

#else

#include <assert.h>

#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Calling conventions and interface method declaration/definition helpers

#ifdef __cplusplus

#define GDK_API                         __stdcall
#define GDK_IFACEMETHOD                 virtual HRESULT GDK_API
#define GDK_IFACEMETHOD_(returnType)    virtual returnType GDK_API
#define GDK_IFACEMETHODIMP              HRESULT GDK_API
#define GDK_IFACEMETHODIMP_(returnType) returnType GDK_API

#endif // __cplusplus

#endif // _GDK_PLATFORM_H_
