// A simple ref counting interface which can be used across DLL boundaries
#pragma once

#include "types.h"
#include <type_traits>
#include <GDK\RefCounted.h>

namespace stde
{
    template <typename Interface>
    class ref_ptr
    {
        static_assert(std::is_base_of<GDK::IRefCounted, Interface>::value, "The 'Interface' template argument must be based on IRefCounted.");
        static_assert(!std::is_pointer<Interface>::value, "The 'Interface' template argument must be included without the pointer syntax. Example ref_ptr<IFoo>, not <IFoo*>");
    public:

        // Construction and destruction

        explicit ref_ptr(_In_opt_ Interface* p = nullptr)
            : p(p)
        {
        }

        ref_ptr(_In_ const ref_ptr& other)
            : p(other.p)
        {
            addref();
        }

        ref_ptr(_In_ ref_ptr&& other)
            : p(other.p)
        {
            other.p = nullptr;
        }

        ~ref_ptr()
        {
            release();
        }

        Interface* get() { return p; }

        // Typecast operators
        operator Interface*() { return p; }
        operator const Interface*() const { return p; }

        // Pointer operator, for use like a normal Interface*, such as smartPointer->Foo()
        Interface* operator->() { return p; }

        // Get pointer & addref
        template <typename T>
        T* as() const
        {
            static_assert(std::is_base_of<T, Interface>::value, "The 'T' template argument must be a base of the 'Interface' template argument.");

            if (p)
            {
                T* t = reinterpret_cast<T*>(p);
                if (t)
                {
                    t->AddRef();
                }
                return t;
            }

            return nullptr;
        }

        // Acquiring a pointer by taking ownership (no addref)

        // Ex: get_object(foo, &my_ref_counted_ptr);
        Interface** operator &()
        {
            return &p;
        }

        // Ex: my_ref_counted_ptr.attach(p); // p is now owned by ref_ptr
        void attach(_In_ Interface* p)
        {
            release();
            this->p = p;
        }

        // Acquiring a pointer with joint ownership (addref)

        ref_ptr& operator= (_In_opt_ Interface* p)
        {
            release();
            this->p = p;
            addref();
            return *this;
        }

        ref_ptr& operator= (_In_ const ref_ptr& other)
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

        ref_ptr& operator= (_In_ ref_ptr&& other)
        {
            if (other.p == p)
            {
                release();
                other.p = nullptr;
                return *this;
            }

            release();
            this->p = other.p;
            other.p = nullptr;
            return *this;
        }

        // giving away ownership (no release)
        Interface* detach()
        {
            Interface* old = p;
            p = nullptr;
            return old;
        }

        // reset

        void reset(_In_opt_ Interface* p = nullptr)
        {
            *this = p;
        }

    private:
        void addref()
        {
            if (p)
            {
                p->AddRef();
            }
        }

        void release()
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

