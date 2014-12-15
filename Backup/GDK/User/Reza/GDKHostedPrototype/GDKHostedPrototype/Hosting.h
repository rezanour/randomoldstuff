#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_HOSTING_H_
#define _GDK_HOSTING_H_

////////////////////////////////////////////////////////////////////////////////
// hosting interfaces that executable hosts must implement

namespace GDK
{
    enum PlatformType
    {
        PlatformType_Windows,
        PlatformType_Console,
        PlatformType_Mobile,
    };

    struct IGDKHost : IRefCounted
    {
        GDK_IMETHOD_(PlatformType) GetPlatformType() const = 0;
        GDK_IMETHOD GetTitle(_Inout_count_(cchMax) wchar_t* name, _In_ size_t cchMax, _Out_ size_t* cchName) = 0;
        GDK_IMETHOD_(void) Exit(_In_ int32_t exitCode) = 0;
    };

    HRESULT GDK_PUBLIC GDK_API ObjectTest();

} // GDK

#endif // _GDI_HOSTING_H_
