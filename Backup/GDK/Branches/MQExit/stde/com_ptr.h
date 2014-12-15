// com_ptr.h
// Smart, ref-counting com pointer, which fully supports STL semantics for containers, etc...

#pragma once

#ifdef WIN32

#include <ObjBase.h>
#include <type_traits>

namespace stde
{
#define VALIDATE_COM_TEMPLATE(x) \
    static_assert(std::is_base_of<IUnknown, Interface>::value, "Template parameter 'Interface' must be an IUnknown derived COM interface"); \
    static_assert(!std::is_pointer<Interface>::value, "Template parameter 'Interface' should be the name directly, not a pointer to it. Examples: Good: <IFoo>, Bad: <IFoo*>");

    // releases an unmanaged com pointer if it's non-null, then sets it to nullptr
    template <typename Interface>
    void __fastcall safe_release(_Inout_ Interface*& p)
    {
        VALIDATE_COM_TEMPLATE(Interface);

        if (p)
        {
            p->Release();
            p = nullptr;
        }
    }

    template <typename Interface>
    class com_ptr sealed
    {
        VALIDATE_COM_TEMPLATE(Interface);

    public:

        // Construction and destruction

        explicit com_ptr(_In_opt_ Interface* p = nullptr)
            : p(p)
        {
            addref();
        }

        com_ptr(_In_ const com_ptr& other)
            : p(other.p)
        {
            addref();
        }

        ~com_ptr()
        {
            release();
        }

        // Typecast operators
        __fastcall operator Interface*() { return p; }
        __fastcall operator const Interface*() const { return p; }

        // Pointer operator, for use like a normal Interface*, such as smartPointer->Foo()
        Interface* __fastcall operator->() { return p; }

        // Acquiring a pointer by taking ownership (no addref)

        // Ex: get_com_object(foo, &my_com_ptr);
        Interface** __fastcall operator &()
        {
            return &p;
        }

        // Ex: my_com_ptr.attach(p); // p is now owned by com_ptr
        void __fastcall attach(_In_ Interface* p)
        {
            release();
            this->p = p;
        }

        // Ex: my_com_ptr.from<IOther>(p); // QI's p for IOther* and gives to my_com_ptr
        template <typename OtherInterface>
        HRESULT __fastcall from(_In_ OtherInterface* pOther)
        {
            VALIDATE_COM_TEMPLATE(OtherInterface);

            if (!pOther)
                return E_INVALIDARG;

            return pOther->QueryInterface(IID_PPV_ARGS(&p));
        }

        // Acquiring a pointer with joint ownership (addref)

        com_ptr& __fastcall operator= (_In_opt_ Interface* p)
        {
            release();
            this->p = p;
            addref();
            return *this;
        }

        com_ptr& __fastcall operator= (_In_ const com_ptr& other)
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

    private:
        void __fastcall addref()
        {
            if (p) p->AddRef();
        }

        void __fastcall release()
        {
            safe_release<Interface>(p);
        }

        Interface* p;
    };
}

#endif

