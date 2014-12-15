#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_REFCOUNTED_H_
#define _GDK_REFCOUNTED_H_

////////////////////////////////////////////////////////////////////////////////
// IRefCounted interface, helper methods, and template class to ease implementation

namespace GDK
{
    // IRefCounted defines a very simple, but effective ref counting scheme used by most types
    struct IRefCounted
    {
        virtual void GDK_API AddRef() = 0;
        virtual void GDK_API Release() = 0;
    };

    // helper methods
    template <typename Ty>
    void SafeRelease(_Inout_ Ty*& p);

    // A template class to simplify implementing ref counted objects
    namespace Details { struct IEmpty {}; struct IEmpty2 {}; };

    template <typename I0 = IRefCounted, typename I1 = Details::IEmpty, typename I2 = Details::IEmpty2>
    class RefCounted : public I0, I1, I2
    {
        static_assert(std::is_base_of<IRefCounted, I0>::value, "First interface in the chain must derive from IRefCounted");

    public:
        // IRefCounted
        void GDK_API AddRef();
        void GDK_API Release();

    protected:
        RefCounted();
        virtual ~RefCounted();

    private:
        // prevent copy construction and copy
        // to keep refs accurate
        RefCounted(const RefCounted&);
        RefCounted& operator= (const RefCounted&);

        uint32_t _refCount;
    };
} // GDK

#include "RefCounted.inl"

#endif // _GDK_REFCOUNTED_H_
