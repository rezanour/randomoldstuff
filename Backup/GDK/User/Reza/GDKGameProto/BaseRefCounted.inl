#pragma once

namespace GDK
{
    template <typename I0, typename I1, typename I2, typename I3>
    inline BaseRefCounted<I0, I1, I2, I3>::BaseRefCounted()
        : _refCount(1)
    {
    }

    template <typename I0, typename I1, typename I2, typename I3>
    inline BaseRefCounted<I0, I1, I2, I3>::~BaseRefCounted()
    {
    }

    template <typename I0, typename I1, typename I2, typename I3>
    inline void BaseRefCounted<I0, I1, I2, I3>::AddRef()
    {
        InterlockedIncrement(&_refCount);
    }

    template <typename I0, typename I1, typename I2, typename I3>
    inline void BaseRefCounted<I0, I1, I2, I3>::Release()
    {
        if (InterlockedDecrement(&_refCount) == 0)
        {
            delete this;
        }
    }
} // GDK
