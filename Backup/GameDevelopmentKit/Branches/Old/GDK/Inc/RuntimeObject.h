#pragma once

namespace GDK
{
    //
    // RuntimeObject is a base template class for implementing IUnknown in XPlatCom.
    // It also uses our private/protected ctor/dtor pattern, so that
    // derived classes implement the static factory approach
    //

    namespace Internal
    {
        GDKINTERFACE IEmpty
        {
            IMPL_GUID(0x5875dde7, 0x3117, 0x456c, 0x85, 0x5e, 0x15, 0x59, 0xab, 0x4d, 0xa8, 0x1c);
        };

        GDKINTERFACE IEmpty2
        {
            IMPL_GUID(0x12a6bc8, 0x10b5, 0x4626, 0x8b, 0xc3, 0x78, 0xa9, 0x24, 0x7c, 0x59, 0x12);
        };

        GDKINTERFACE IEmpty3
        {
            IMPL_GUID(0xe887585e, 0x76a3, 0x464a, 0xbb, 0x83, 0xfe, 0xf0, 0xee, 0x9c, 0xe9, 0xd2);
        };
    }

    template <class T>
    Microsoft::WRL::ComPtr<T> Make()
    {
        Microsoft::WRL::ComPtr<T> obj;
        obj.Attach(new T());
        return obj;
    }

    template <class T, typename A0>
    Microsoft::WRL::ComPtr<T> Make(A0 a0)
    {
        Microsoft::WRL::ComPtr<T> obj;
        obj.Attach(new T(a0));
        return obj;
    }

    template <class T, typename A0, typename A1>
    Microsoft::WRL::ComPtr<T> Make(A0 a0, A1 a1)
    {
        Microsoft::WRL::ComPtr<T> obj;
        obj.Attach(new T(a0, a1));
        return obj;
    }

    template <class T, typename A0, typename A1, typename A2>
    Microsoft::WRL::ComPtr<T> Make(A0 a0, A1 a1, A2 a2)
    {
        Microsoft::WRL::ComPtr<T> obj;
        obj.Attach(new T(a0, a1, a2));
        return obj;
    }

    template <class T, typename A0, typename A1, typename A2, typename A3>
    Microsoft::WRL::ComPtr<T> Make(A0 a0, A1 a1, A2 a2, A3 a3)
    {
        Microsoft::WRL::ComPtr<T> obj;
        obj.Attach(new T(a0, a1, a2, a3));
        return obj;
    }

    template <class T, typename A0, typename A1, typename A2, typename A3, typename A4>
    Microsoft::WRL::ComPtr<T> Make(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4)
    {
        Microsoft::WRL::ComPtr<T> obj;
        obj.Attach(new T(a0, a1, a2, a3, a4));
        return obj;
    }

    class DontUseNewUseMake
    {
    protected:
        void operator delete (void* ptr)
        {
            if (ptr)
            {
                free(ptr);
            }
        }

        void operator delete (void* ptr, const std::nothrow_t& nothrow_constant);
        void operator delete (void* ptr, void* voidptr2);

        void operator delete[] (void* ptr);
        void operator delete[] (void* ptr, const std::nothrow_t& nothrow_constant);
        void operator delete[] (void* ptr, void* voidptr2);

    private:
        void* operator new (size_t size)
        {
            return malloc(size);
        }

        void* operator new (size_t size, const std::nothrow_t& nothrow_constant);
        void* operator new (size_t size, void* ptr);

        void* operator new[] (std::size_t size);
        void* operator new[] (std::size_t size, const std::nothrow_t& nothrow_constant);
        void* operator new[] (std::size_t size, void* ptr);

        template <class T>
        friend Microsoft::WRL::ComPtr<T> Make();

        template <class T, typename A0>
        friend Microsoft::WRL::ComPtr<T> Make(A0 a0);

        template <class T, typename A0, typename A1>
        friend Microsoft::WRL::ComPtr<T> Make(A0 a0, A1 a1);

        template <class T, typename A0, typename A1, typename A2>
        friend Microsoft::WRL::ComPtr<T> Make(A0 a0, A1 a1, A2 a2);

        template <class T, typename A0, typename A1, typename A2, typename A3>
        friend Microsoft::WRL::ComPtr<T> Make(A0 a0, A1 a1, A2 a2, A3 a3);

        template <class T, typename A0, typename A1, typename A2, typename A3, typename A4>
        friend Microsoft::WRL::ComPtr<T> Make(A0 a0, A1 a1, A2 a2, A3 a3, A4 a4);
    };

    template <typename Interface0 = IUnknown, typename Interface1 = Internal::IEmpty, typename Interface2 = Internal::IEmpty2, typename Interface3 = Internal::IEmpty3>
    class RuntimeObject : public DontUseNewUseMake, public Interface0, public Interface1, public Interface2, public Interface3
    {
    public:
        virtual HRESULT GDKAPI QueryInterface(_In_ REFIID riid, _COM_Outptr_ void** ppvObject)
        {
            if (!ppvObject)
            {
                return E_POINTER;
            }

            *ppvObject = nullptr;
            HRESULT hr = E_NOINTERFACE;

            if (IsEqualIID(riid, __uuidof(Interface0)))
            {
                *ppvObject = static_cast<Interface0*>(this);
            }
            else if (IsEqualIID(riid, __uuidof(Interface1)))
            {
                *ppvObject = static_cast<Interface1*>(this);
            }
            else if (IsEqualIID(riid, __uuidof(Interface2)))
            {
                *ppvObject = static_cast<Interface2*>(this);
            }
            else if (IsEqualIID(riid, __uuidof(Interface3)))
            {
                *ppvObject = static_cast<Interface3*>(this);
            }

            if (*ppvObject)
            {
                AddRef();
                hr = S_OK;
            }

            return hr;
        }

        virtual ULONG GDKAPI AddRef(void)
        {
            return InterlockedIncrement(&_ref);
        }

        virtual ULONG GDKAPI Release(void)
        {
            ULONG ref = InterlockedDecrement(&_ref);
            if (ref == 0)
            {
                delete this;
            }
            return ref;
        }

    protected:
        RuntimeObject() : _ref(1) {}
        virtual ~RuntimeObject() {}

    private:
        // RuntimeObjects should not be copyable
        RuntimeObject(const RuntimeObject&);
        RuntimeObject& operator= (const RuntimeObject&);

        ULONG _ref;
    };

} // GDK
