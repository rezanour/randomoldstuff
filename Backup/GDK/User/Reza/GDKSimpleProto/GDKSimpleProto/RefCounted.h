#pragma once

#include "IRefCounted.h"

namespace GDK
{
    namespace _internal_
    {
        struct IEmpty {};
        struct IEmpty2 {};
        struct IEmpty3 {};
    };

    template <typename I0, typename I1 = _internal_::IEmpty, typename I2 = _internal_::IEmpty2, typename I3 = _internal_::IEmpty3>
    class RefCounted : public I0, public I1, public I2, public I3
    {
        static_assert(std::is_base_of<IRefCounted, I0>::value, "I0 must derive from IRefCounted");

    public:
        // IRefCounted
        void AddRef();
        void Release();

    protected:
        RefCounted();
        virtual ~RefCounted();

    private:
        uint32_t _refCount;
    };
} // GDK

#include "RefCounted.inl"
