#pragma once

#ifndef WIN32

#   include <assert.h>
#   include <libkern/OSAtomic.h>

#   define UNREFERENCED_PARAMETER(p)
#   define _Inout_
#   define _Post_null_
#   define _In_
#   define _In_z_
#   define _In_opt_z_
#   define _In_opt_
#   define _Outptr_
#   define _Outptr_result_maybenull_
#   define _Outptr_result_nullonfailure_
#   define _Out_
#   define _Printf_format_string_
#   define _COM_Outptr_
#   define _Inout_updates_to_(a, b)
#   define _In_opt_z_count_(a)
#   define __FILEW__ L"##__FILE__"
#   define __WRL_ASSERT__(x) assert(x)
#   define InterlockedIncrement(x) OSAtomicIncrement64(reinterpret_cast<int64_t *>(x))
#   define InterlockedDecrement(x) OSAtomicDecrement64(reinterpret_cast<int64_t *>(x))

#   define FAILED(hr)       (hr != 0)
#   define SUCCEEDED(hr)    (hr == 0)
#   define S_OK             0x00000000
#   define E_NOTIMPL        0x80004001
#   define E_NOINTERFACE    0x80004002
#   define E_POINTER        0x80004003
#   define E_ABORT          0x80004004
#   define E_FAIL           0x80004005
#   define E_UNEXPECTED     0x8000FFFF
#   define E_ACCESSDENIED   0x80070005
#   define E_HANDLE         0x80070006
#   define E_OUTOFMEMORY    0x8007000E
#   define E_INVALIDARG     0x80070057

#   define ERROR_NOT_FOUND      0x80070490
#   define ERROR_INVALID_DATA   0x8007000d

#   define _countof(a) (sizeof(a)/sizeof(*(a)))

#   define ZeroMemory(p, s) (memset(p, 0, s))

#   define FACILITY_WIN32 7

    typedef long HRESULT;
    typedef long LONG;
    typedef unsigned char byte;
    typedef unsigned int DWORD;
    typedef unsigned int UINT;

    typedef struct _RECT {
        LONG left;
        LONG top;
        LONG right;
        LONG bottom;
    } RECT, *PRECT;

    inline HRESULT HRESULT_FROM_WIN32(unsigned long x) {
        return (HRESULT)(x) <= 0 ? (HRESULT)(x) : (HRESULT) (((x) & 0x0000FFFF) | (FACILITY_WIN32 << 16) | 0x80000000);
    }

#endif // #ifndef WIN32

#define TXT(x) L ## #x

#include <XPlatCom.h>
#include <XPlatWrl.h>

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#include <stdio.h>
#include <wchar.h>

#include <type_traits>
#include <functional>

typedef unsigned char byte_t;
typedef char sbyte_t;

#define GDKAPI __stdcall
#define GDKINTERFACE struct __declspec(novtable)

template <typename T>
inline void SafeDelete(_Inout_ _Post_null_ T*& p) { if (p) { delete p; p = nullptr; } }

template <typename T>
inline void SafeDeleteArray(_Inout_ _Post_null_ T*& p) { if (p) { delete [] p; p = nullptr; } }

template <typename T>
inline void SafeRelease(_Inout_ _Post_null_ T*& p) { if (p) { p->Release(); p = nullptr; } }

#ifdef WIN32
extern "C" void __cdecl DebugOut(_In_z_ _Printf_format_string_ const wchar_t* format, ...);
#else
// Kind of a hack just to get things working. Probably should implement this better.
inline void __cdecl DebugOut(_In_z_ _Printf_format_string_ const wchar_t* format, ...)
{
    va_list args;
    va_start(args, format);
    vfwprintf(stderr, format, args);
    va_end(args);
}
#endif

#include <GDKError.h>
