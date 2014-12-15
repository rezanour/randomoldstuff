#pragma once

#include <type_traits>

namespace Lucid3D
{
    struct IRefCounted
    {
        virtual void __stdcall AddRef() = 0;
        virtual void __stdcall Release() = 0;
    };

    template <typename Ty>
    void SafeRelease(_Inout_ Ty*& p)
    {
        static_assert(std::is_base_of<IRefCounted, Ty>::value, "SafeRelease is for IRefCounted based types");
        if (p)
        {
            p->Release();
            p = nullptr;
        }
    }
}
