#pragma once

namespace GDK
{
    template <typename I0, typename I1, typename I2, typename I3>
    inline RefCounted<I0, I1, I2, I3>::RefCounted()
        : _refCount(1)
    {
    }

    template <typename I0, typename I1, typename I2, typename I3>
    inline RefCounted<I0, I1, I2, I3>::~RefCounted()
    {
    }

    template <typename I0, typename I1, typename I2, typename I3>
    inline void RefCounted<I0, I1, I2, I3>::AddRef()
    {
        InterlockedIncrement(&_refCount);
    }

    template <typename I0, typename I1, typename I2, typename I3>
    inline void RefCounted<I0, I1, I2, I3>::Release()
    {
        if (InterlockedDecrement(&_refCount) == 0)
        {
            delete this;
        }
    }
} // GDK
