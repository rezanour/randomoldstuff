#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_REFCOUNTED_H_
#define _GDK_REFCOUNTED_H_

#include "Platform.h"

namespace GDK
{
    struct IRefCounted
    {
        GDK_IMETHOD_(void) AddRef() = 0;
        GDK_IMETHOD_(void) Release() = 0;
    };

    namespace _Internal { struct IEmpty {}; struct IEmpty2 {}; }

    template <typename I1 = IRefCounted, typename I2 = _Internal::IEmpty, typename I3 = _Internal::IEmpty2>
    class RefCounted : public I1, public I2, public I3
    {
        static_assert(std::is_base_of<IRefCounted, I1>::value, "First interface template argument must be derived from IRefCounted!");

    public:
        GDK_IMETHOD_(void) AddRef();
        GDK_IMETHOD_(void) Release();

    protected:
        RefCounted();
        virtual ~RefCounted();

    private:
        // non-copyable
        RefCounted(const RefCounted&);
        RefCounted& operator= (const RefCounted&);

        // non-movable
        RefCounted(RefCounted&&);
        RefCounted& operator= (RefCounted&&);

        long _refCount;
    };

    template <typename Ty>
    inline void SafeRelease(_Inout_ Ty*& p)
    {
        if (p)
        {
            p->Release();
            p = nullptr;
        }
    }
}

#include "RefCounted.inl"

#endif // _GDK_REFCOUNTED_H_
