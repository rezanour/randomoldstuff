// A simple ref counting interface which can be used across DLL boundaries
#pragma once

#include "types.h"
#include <type_traits>
#include <GDK\RefCounted.h>

namespace stde
{
    template <typename Interface>
    class ref_counted_ptr
    {
        static_assert(std::is_base_of<GDK::IRefCounted, Interface>::value, "The 'Interface' template argument must be based on IRefCounted.");
        static_assert(!std::is_pointer<Interface>::value, "The 'Interface' template argument must be included without the pointer syntax. Example ref_counted_ptr<IFoo>, not <IFoo*>");
    public:

        // Construction and destruction

        explicit ref_counted_ptr(_In_opt_ Interface* p = nullptr)
            : p(p)
        {
        }

        ref_counted_ptr(_In_ const ref_counted_ptr& other)
            : p(other.p)
        {
            addref();
        }

        ~ref_counted_ptr()
        {
            release();
        }

        Interface* get() { return p; }

        // Typecast operators
        __fastcall operator Interface*() { return p; }
        __fastcall operator const Interface*() const { return p; }

        // Pointer operator, for use like a normal Interface*, such as smartPointer->Foo()
        Interface* __fastcall operator->() { return p; }

        // Get pointer & addref
        template <typename T>
        T* as() const
        {
            static_assert(std::is_base_of<T, Interface>::value, "The 'T' template argument must be a base of the 'Interface' template argument.");

            if (p)
            {
                T* t = reinterpret_cast<T*>(p);
                t->AddRef();
                return t;
            }

            return nullptr;
        }

        // Acquiring a pointer by taking ownership (no addref)

        // Ex: get_object(foo, &my_ref_counted_ptr);
        Interface** __fastcall operator &()
        {
            return &p;
        }

        // Ex: my_ref_counted_ptr.attach(p); // p is now owned by ref_counted_ptr
        void __fastcall attach(_In_ Interface* p)
        {
            release();
            this->p = p;
        }

        // Acquiring a pointer with joint ownership (addref)

        ref_counted_ptr& __fastcall operator= (_In_opt_ Interface* p)
        {
            release();
            this->p = p;
            addref();
            return *this;
        }

        ref_counted_ptr& __fastcall operator= (_In_ const ref_counted_ptr& other)
        {
            if (other.p == p)
            {
                return *this;
            }

            release();
            this->p = other.p;
            addref();
            return *this;
        }

        // giving away ownership (no release)
        Interface* __fastcall detach()
        {
            Interface* old = p;
            p = nullptr;
            return old;
        }

        // reset

        void __fastcall reset(_In_opt_ Interface* p = nullptr)
        {
            *this = p;
        }

    private:
        void __fastcall addref()
        {
            if (p)
            {
                p->AddRef();
            }
        }

        void __fastcall release()
        {
            if (p)
            {
                p->Release();
                p = nullptr;
            }
        }

        Interface* p;
    };
}

