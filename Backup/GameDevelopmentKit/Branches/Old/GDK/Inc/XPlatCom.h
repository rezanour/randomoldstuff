#ifndef _XPLATCOM_H_
#define _XPLATCOM_H_

#ifdef WIN32

#include <guiddef.h>

#define COM_NO_WINDOWS_H
#include <Unknwn.h>

#else // WIN32

#include <memory.h>

typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} GUID;

#ifndef DECLSPEC_SELECTANY
#if (_MSC_VER >= 1100)
#define DECLSPEC_SELECTANY  __declspec(selectany)
#else
#define DECLSPEC_SELECTANY
#endif
#endif

#define EXTERN_C    extern "C"

#ifdef DEFINE_GUID
#undef DEFINE_GUID
#endif

#ifdef INITGUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#else
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID name
#endif // INITGUID

typedef GUID *LPGUID;
typedef const GUID *LPCGUID;
typedef GUID IID;
typedef IID *LPIID;

DEFINE_GUID(GUID_NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

#define IID_NULL            GUID_NULL
#define IsEqualIID(riid1, riid2) IsEqualGUID(riid1, riid2)

typedef GUID CLSID;
typedef CLSID *LPCLSID;

#define CLSID_NULL          GUID_NULL
#define IsEqualCLSID(rclsid1, rclsid2) IsEqualGUID(rclsid1, rclsid2)

#define REFGUID const GUID &
#define REFIID const IID &
#define REFCLSID const IID &

__inline int IsEqualGUID(REFGUID rguid1, REFGUID rguid2)
{
    return !memcmp(&rguid1, &rguid2, sizeof(GUID));
}

// Same type, different name

#define IsEqualIID(riid1, riid2) IsEqualGUID(riid1, riid2)
#define IsEqualCLSID(rclsid1, rclsid2) IsEqualGUID(rclsid1, rclsid2)


__inline bool operator==(REFGUID guidOne, REFGUID guidOther)
{
    return !!IsEqualGUID(guidOne,guidOther);
}

__inline bool operator!=(REFGUID guidOne, REFGUID guidOther)
{
    return !(guidOne == guidOther);
}

DEFINE_GUID(IID_IUnknown, 0x00000000, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);

// common typedefs used by COM code
typedef long HRESULT;
typedef unsigned long ULONG;

struct IUnknown
{
    virtual HRESULT __stdcall QueryInterface(_In_ REFIID riid, _COM_Outptr_ void** ppvObject) = 0;
    virtual ULONG __stdcall AddRef(void) = 0;
    virtual ULONG __stdcall Release(void) = 0;
};

#endif // WIN32

#define IMPL_GUID(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    public: \
    static inline REFGUID GetGUID() { \
        static GUID guid = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }; \
        return guid; \
    }

namespace GDK
{
    template <typename Interface>
    inline REFGUID uuidof()
    {
        return Interface::GetGUID();
    }
}

#define DECL_NON_GDK_UUIDOF(x, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
struct x; \
template <> \
inline REFGUID GDK::uuidof<x>() \
{ \
    static GUID guid = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }; \
    return guid; \
}

DECL_NON_GDK_UUIDOF(IUnknown, 0x00000000, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46)
DECL_NON_GDK_UUIDOF(IWeakReference, 0x00000037, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46)

#define __uuidof(x) GDK::uuidof<x>()

#endif // _XPLATCOM_H_