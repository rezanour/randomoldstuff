#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_REFCOUNTED_INL_
#define _GDK_REFCOUNTED_INL_

namespace GDK
{
    template <typename I1, typename I2, typename I3>
    inline RefCounted<I1, I2, I3>::RefCounted()
        : _refCount(1)
    {
    }

    template <typename I1, typename I2, typename I3>
    inline RefCounted<I1, I2, I3>::~RefCounted()
    {
        assert(_refCount == 0 && "Object deleted before ref count reached 0");
    }

    template <typename I1, typename I2, typename I3>
    inline void RefCounted<I1, I2, I3>::AddRef()
    {
        AtomicIncrement(&_refCount);
    }

    template <typename I1, typename I2, typename I3>
    inline void RefCounted<I1, I2, I3>::Release()
    {
        if (AtomicDecrement(&_refCount) == 0)
        {
            delete this;
        }
    }
}

#endif // _GDK_REFCOUNTED_INL_
