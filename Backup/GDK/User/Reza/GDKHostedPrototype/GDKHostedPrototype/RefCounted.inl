#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_REFCOUNTED_INL_
#define _GDK_REFCOUNTED_INL_

namespace GDK
{
    template <typename Ty>
    inline void SafeRelease(_Inout_ Ty*& p)
    {
        static_assert(std::is_base_of<IRefCounted, Ty>::value, "template parameter must be a descendent of IRefCounted");
        if (p)
        {
            p->Release();
            p = nullptr;
        }
    }

    template <typename I0, typename I1, typename I2>
    inline RefCounted<I0, I1, I2>::RefCounted()
        : _refCount(1)
    {
    }

    template <typename I0, typename I1, typename I2>
    inline RefCounted<I0, I1, I2>::~RefCounted()
    {
    }

    template <typename I0, typename I1, typename I2>
    inline void RefCounted<I0, I1, I2>::AddRef()
    {
        AtomicIncrement(&_refCount);
    }

    template <typename I0, typename I1, typename I2>
    inline void RefCounted<I0, I1, I2>::Release()
    {
        if (AtomicDecrement(&_refCount) == 0)
        {
            delete this;
        }
    }
} // GDK

#endif _GDK_REFCOUNTED_INL_