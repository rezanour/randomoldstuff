// IRefCounted is a basic ref counting interface which most SDK types derive from
#pragma once

#include "Platform.h"
#include <cassert>
#include <type_traits>

namespace GDK
{
    struct IRefCounted
    {
        GDK_METHOD_(long) AddRef() = 0;
        GDK_METHOD_(long) Release() = 0;
    };

    namespace _Internal
    {
        struct IEmpty {};
        struct IEmpty2 {};
    }

    template <typename Interface1, typename Interface2 = _Internal::IEmpty, typename Interface3 = _Internal::IEmpty2>
    class RefCountedBase : public Interface1, public Interface2, public Interface3
    {
        static_assert(std::is_base_of<IRefCounted, Interface1>::value, "First interface template argument must be derived from IRefCounted!");

    public:
        RefCountedBase()
            : _cRef(1)
        {
        }

        virtual ~RefCountedBase()
        {
            // This assert is normally triggered if you stack allocate the object. RefCountedBase assumes heap allocation always
            assert(_cRef == 0);
        }

        // IRefCounted
        GDK_METHOD_(long) AddRef()
        {
            return InterlockedIncrement(&_cRef);
        }

        GDK_METHOD_(long) Release()
        {
            if (InterlockedDecrement(&_cRef) <= 0)
            {
                delete this;
                return 0;
            }
            return _cRef;
        }

    private:
        long _cRef;
    };
}
