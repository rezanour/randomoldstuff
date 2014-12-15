#ifndef _XPLATWRL_H_
#define _XPLATWRL_H_

#if defined(WIN32) && defined(_MSC_VER) && _MSC_VER > 1100

#include <wrl\client.h>

#else // WIN32 && _MSC_VER > 1100

#pragma warning(push)

// 'class' : default constructor could not be generated
#pragma warning(disable: 4510)
// object 'class' can never be instantiated - user-defined constructor required
#pragma warning(disable: 4610)
//'derived class' : destructor could not be generated because a base class destructor is inaccessible
#pragma warning(disable: 4624)
// Warnings 4510 and 4610 are generated because of as<>() helper object
// that don't require any constructors thus disabling those warnings
// Warning 4624 is generated if the object has private destructor and the object cannot be created on the stack
// Comptr does not require the object to be created on the stack thus disabling the warning

namespace Microsoft {
namespace WRL {

namespace Details
{
    struct BoolStruct
    {
        int Member;
    };

    typedef int BoolStruct::* BoolType;

    template <bool b, typename T = void>
    struct EnableIf
    {
    };

    template <typename T>
    struct EnableIf<true, T>
    {
        typedef T type;
    };

    template <typename T1, typename T2>
    struct IsSame
    {
        static const bool value = false;
    };

    template <typename T1>
    struct IsSame<T1, T1>
    {
        static const bool value = true;
    };

    template<class T>
    struct RemoveReference
    {
        typedef T Type;
    };

    template<class T>
    struct RemoveReference<T&>
    {
        typedef T Type;
    };

    template<class T>
    struct RemoveReference<T&&>
    {
        typedef T Type;
    };

    template<class T>
    inline typename RemoveReference<T>::Type&& Move(_Inout_ T&& arg) throw()
    {
        return ((typename RemoveReference<T>::Type&&)arg);
    }

    template<class T>
    inline void Swap(_Inout_ T& left, _Inout_ T& right) throw()
    {
        T tmp = Move(left);
        left = Move(right);
        right = Move(tmp);
    }

    // Disables template argument deduction from Forward helper
    template<class T>
    struct Identity
    {
        // Map T to type unchanged
        typedef T Type;
    };

    template<class T> 
    inline T&& Forward(typename Identity<T>::Type& arg) throw()
    {
        // Forward arg, given explicitly specified Type parameter
        return (T&&)arg;
    }

    template <typename Base, typename Derived>
    struct IsBaseOfStrict
    {
        static const bool value = __is_base_of(Base, Derived);
    };

    template <typename Base>
    struct IsBaseOfStrict<Base, Base>
    {
        static const bool value = false;
    };

    // Helper object that makes IUnknown methods private
    template <typename T>
    class RemoveIUnknownBase : public T
    {
    private:
        // STDMETHOD macro implies virtual.
        // ComPtr can be used with any class that implements the 3 methods of IUnknown.
        // ComPtr does not require these methods to be virtual.
        // When ComPtr is used with a class without a virtual table, marking the functions
        // as virtual in this class adds unnecessary overhead.
        HRESULT __stdcall QueryInterface(REFIID riid, _COM_Outptr_ void **ppvObject);
        ULONG __stdcall AddRef();
        ULONG __stdcall Release();
    };

    template<typename T>
    struct RemoveIUnknown
    {
        typedef RemoveIUnknownBase<T> ReturnType;
    };

    template<typename T>
    struct RemoveIUnknown<const T>
    {
        typedef const RemoveIUnknownBase<T> ReturnType;
    };

    template <typename T> // T should be the ComPtr<T> or a derived type of it, not just the interface
    class ComPtrRefBase
    {
    public:
        typedef typename T::InterfaceType InterfaceType;

        operator IUnknown**() const throw()
        {
            static_assert(__is_base_of(IUnknown, InterfaceType), "Invalid cast: InterfaceType does not derive from IUnknown");
            return reinterpret_cast<IUnknown**>(ptr_->ReleaseAndGetAddressOf());
        }

    protected:
        T* ptr_;
    };

    template <typename T>
    class ComPtrRef : public Details::ComPtrRefBase<T> // T should be the ComPtr<T> or a derived type of it, not just the interface
    {
    public:
        typedef typename Details::ComPtrRefBase<T>::InterfaceType InterfaceType;

        ComPtrRef(_In_opt_ T* ptr) throw()
        {
             ComPtrRefBase<T>::ptr_ = ptr;
        }

        // Conversion operators
        operator void**() const throw()
        {
            return reinterpret_cast<void**>(this->ptr_->ReleaseAndGetAddressOf());
        }

        // This is our operator ComPtr<U> (or the latest derived class from ComPtr (e.g. WeakRef))
        operator T*() throw()
        {
            *this->ptr_ = nullptr;
            return this->ptr_;
        }

        // We define operator InterfaceType**() here instead of on ComPtrRefBase<T>, since
        // if InterfaceType is IUnknown or IInspectable, having it on the base will colide.
        operator InterfaceType**() throw()
        {
            return this->ptr_->ReleaseAndGetAddressOf();
        }

        // This is used for IID_PPV_ARGS in order to do __uuidof(**(ppType)).
        // It does not need to clear  ptr_ at this point, it is done at IID_PPV_ARGS_Helper(ComPtrRef&) later in this file.
        InterfaceType* operator *() throw()
        {
            return this->ptr_->Get();
        }

        // Explicit functions
        InterfaceType* const * GetAddressOf() const throw()
        {
            return this->ptr_->GetAddressOf();
        }

        InterfaceType** ReleaseAndGetAddressOf() throw()
        {
            return this->ptr_->ReleaseAndGetAddressOf();
        }
    };
} // namespace Details

template <typename T>
class ComPtr
{
public:
    typedef T InterfaceType;

protected:
    InterfaceType *ptr_;
    template<class U> friend class ComPtr;

    void InternalAddRef() const throw()
    {
        if (ptr_ != nullptr)
        {
            ptr_->AddRef();
        }
    }

    unsigned long InternalRelease() throw()
    {
        unsigned long ref = 0;
        T* temp = ptr_;

        if (temp != nullptr)
        {
            ptr_ = nullptr;
            ref = temp->Release();
        }

        return ref;
    }

public:
#pragma region constructors
    ComPtr() throw() : ptr_(nullptr)
    {
    }

    ComPtr(decltype(__nullptr)) throw() : ptr_(nullptr)
    {
    }

    template<class U>
    ComPtr(_In_opt_ U *other) throw() : ptr_(other)
    {
        InternalAddRef();
    }

    ComPtr(const ComPtr& other) throw() : ptr_(other.ptr_)
    {
        InternalAddRef();
    }

    // copy ctor that allows to instanatiate class when U* is convertible to T*
    template<class U>
    ComPtr(const ComPtr<U> &other) throw() :
        ptr_(other.ptr_)
    {
        InternalAddRef();
    }

    ComPtr(_Inout_ ComPtr &&other) throw() : ptr_(nullptr)
    {
        if (this != reinterpret_cast<ComPtr*>(&reinterpret_cast<byte&>(other)))
        {
            Swap(other);
        }
    }

    // Move ctor that allows instantiation of a class when U* is convertible to T*
    template<class U>
    ComPtr(_Inout_ ComPtr<U>&& other
#ifdef WIN32
           , typename Details::EnableIf<__is_convertible_to(U*, T*), void *>::type * = 0
#endif
           ) throw() :
        ptr_(other.ptr_)
    {
        other.ptr_ = nullptr;
    }
#pragma endregion

#pragma region destructor
    ~ComPtr() throw()
    {
        InternalRelease();
    }
#pragma endregion

#pragma region assignment
    ComPtr& operator=(decltype(__nullptr)) throw()
    {
        InternalRelease();
        return *this;
    }

    ComPtr& operator=(_In_opt_ T *other) throw()
    {
        if (ptr_ != other)
        {
            ComPtr(other).Swap(*this);
        }
        return *this;
    }

    template <typename U>
    ComPtr& operator=(_In_opt_ U *other) throw()
    {
        ComPtr(other).Swap(*this);
        return *this;
    }

    ComPtr& operator=(const ComPtr &other) throw()
    {
        if (ptr_ != other.ptr_)
        {
            ComPtr(other).Swap(*this);
        }
        return *this;
    }

    template<class U>
    ComPtr& operator=(const ComPtr<U>& other) throw()
    {
        ComPtr(other).Swap(*this);
        return *this;
    }

    ComPtr& operator=(_Inout_ ComPtr &&other) throw()
    {
        ComPtr(static_cast<ComPtr&&>(other)).Swap(*this);
        return *this;
    }

    template<class U>
    ComPtr& operator=(_Inout_ ComPtr<U>&& other) throw()
    {
        ComPtr(static_cast<ComPtr<U>&&>(other)).Swap(*this);
        return *this;
    }
#pragma endregion

#pragma region modifiers
    void Swap(_Inout_ ComPtr&& r) throw()
    {
        T* tmp = ptr_;
        ptr_ = r.ptr_;
        r.ptr_ = tmp;
    }

    void Swap(_Inout_ ComPtr& r) throw()
    {
        T* tmp = ptr_;
        ptr_ = r.ptr_;
        r.ptr_ = tmp;
    }
#pragma endregion

    operator Details::BoolType() const throw()
    {
        return Get() != nullptr ? &Details::BoolStruct::Member : nullptr;
    }

    T* Get() const throw()
    {
        return ptr_;
    }

    typename Details::RemoveIUnknown<InterfaceType>::ReturnType* operator->() const throw()
    {
        return static_cast<typename Details::RemoveIUnknown<InterfaceType>::ReturnType*>(ptr_);
    }

    /*
    Details::ComPtrRef<ComPtr<T>> operator&() throw()
    {
        return Details::ComPtrRef<ComPtr<T>>(this);
    }

    const Details::ComPtrRef<const ComPtr<T>> operator&() const throw()
    {
        return Details::ComPtrRef<const ComPtr<T>>(this);
    }
     */

    T* const* GetAddressOf() const throw()
    {
        return &ptr_;
    }

    T** GetAddressOf() throw()
    {
        return &ptr_;
    }

    T** ReleaseAndGetAddressOf() throw()
    {
        InternalRelease();
        return &ptr_;
    }

    T* Detach() throw()
    {
        T* ptr = ptr_;
        ptr_ = nullptr;
        return ptr;
    }

    void Attach(_In_opt_ InterfaceType* other) throw()
    {
        if (ptr_ != nullptr)
        {
            auto ref = ptr_->Release();
            // Attaching to the same object only works if duplicate references are being coalesced. Otherwise
            // re-attaching will cause the pointer to be released and may cause a crash on a subsequent dereference.
            __WRL_ASSERT__(ref != 0 || ptr_ != other);
        }

        ptr_ = other;
    }

    unsigned long Reset()
    {
        return InternalRelease();
    }

    HRESULT CopyTo(_Outptr_result_maybenull_ InterfaceType** ptr) const throw()
    {
        InternalAddRef();
        *ptr = ptr_;
        return S_OK;
    }

    HRESULT CopyTo(REFIID riid, _Outptr_result_nullonfailure_ void** ptr) const throw()
    {
        return ptr_->QueryInterface(riid, ptr);
    }

    template<typename U>
    HRESULT CopyTo(_Outptr_result_nullonfailure_ U** ptr) const throw()
    {
        return ptr_->QueryInterface(__uuidof(U), reinterpret_cast<void**>(ptr));
    }

    // query for U interface
    template<typename U>
    HRESULT As(_Inout_ Details::ComPtrRef<ComPtr<U>> p) const throw()
    {
        return ptr_->QueryInterface(__uuidof(U), p);
    }

    // query for U interface
    template<typename U>
    HRESULT As(_Out_ ComPtr<U>* p) const throw()
    {
        return ptr_->QueryInterface(__uuidof(U), reinterpret_cast<void**>(p->ReleaseAndGetAddressOf()));
    }

    // query for riid interface and return as IUnknown
    HRESULT AsIID(REFIID riid, _Out_ ComPtr<IUnknown>* p) const throw()
    {
        return ptr_->QueryInterface(riid, reinterpret_cast<void**>(p->ReleaseAndGetAddressOf()));
    }
};    // ComPtr

// Comparation operators - don't compare COM object identity
template<class T, class U>
bool operator==(const ComPtr<T>& a, const ComPtr<U>& b) throw()
{
    static_assert(__is_base_of(T, U) || __is_base_of(U, T), "'T' and 'U' pointers must be comparable");
    return a.Get() == b.Get();
}

template<class T>
bool operator==(const ComPtr<T>& a, decltype(__nullptr)) throw()
{
    return a.Get() == nullptr;
}

template<class T>
bool operator==(decltype(__nullptr), const ComPtr<T>& a) throw()
{
    return a.Get() == nullptr;
}

template<class T, class U>
bool operator!=(const ComPtr<T>& a, const ComPtr<U>& b) throw()
{
    static_assert(__is_base_of(T, U) || __is_base_of(U, T), "'T' and 'U' pointers must be comparable");
    return a.Get() != b.Get();
}

template<class T>
bool operator!=(const ComPtr<T>& a, decltype(__nullptr)) throw()
{
    return a.Get() != nullptr;
}

template<class T>
bool operator!=(decltype(__nullptr), const ComPtr<T>& a) throw()
{
    return a.Get() != nullptr;
}

template<class T, class U>
bool operator<(const ComPtr<T>& a, const ComPtr<U>& b) throw()
{
    static_assert(__is_base_of(T, U) || __is_base_of(U, T), "'T' and 'U' pointers must be comparable");
    return a.Get() < b.Get();
}

//// Details::ComPtrRef comparisons
template<class T, class U>
bool operator==(const Details::ComPtrRef<ComPtr<T>>& a, const Details::ComPtrRef<ComPtr<U>>& b) throw()
{
    static_assert(__is_base_of(T, U) || __is_base_of(U, T), "'T' and 'U' pointers must be comparable");
    return a.GetAddressOf() == b.GetAddressOf();
}

template<class T>
bool operator==(const Details::ComPtrRef<ComPtr<T>>& a, decltype(__nullptr)) throw()
{
    return a.GetAddressOf() == nullptr;
}

template<class T>
bool operator==(decltype(__nullptr), const Details::ComPtrRef<ComPtr<T>>& a) throw()
{
    return a.GetAddressOf() == nullptr;
}

template<class T>
bool operator==(const Details::ComPtrRef<ComPtr<T>>& a, void* b) throw()
{
    return a.GetAddressOf() == b;
}

template<class T>
bool operator==(void* b, const Details::ComPtrRef<ComPtr<T>>& a) throw()
{
    return a.GetAddressOf() == b;
}

template<class T, class U>
bool operator!=(const Details::ComPtrRef<ComPtr<T>>& a, const Details::ComPtrRef<ComPtr<U>>& b) throw()
{
    static_assert(__is_base_of(T, U) || __is_base_of(U, T), "'T' and 'U' pointers must be comparable");
    return a.GetAddressOf() != b.GetAddressOf();
}

template<class T>
bool operator!=(const Details::ComPtrRef<ComPtr<T>>& a, decltype(__nullptr)) throw()
{
    return a.GetAddressOf() != nullptr;
}

template<class T>
bool operator!=(decltype(__nullptr), const Details::ComPtrRef<ComPtr<T>>& a) throw()
{
    return a.GetAddressOf() != nullptr;
}

template<class T>
bool operator!=(const Details::ComPtrRef<ComPtr<T>>& a, void* b) throw()
{
    return a.GetAddressOf() != b;
}

template<class T>
bool operator!=(void* b, const Details::ComPtrRef<ComPtr<T>>& a) throw()
{
    return a.GetAddressOf() != b;
}

template<class T, class U>
bool operator<(const Details::ComPtrRef<ComPtr<T>>& a, const Details::ComPtrRef<ComPtr<U>>& b) throw()
{
    static_assert(__is_base_of(T, U) || __is_base_of(U, T), "'T' and 'U' pointers must be comparable");
    return a.GetAddressOf() < b.GetAddressOf();
}

}} // namespace Microsoft::WRL

#pragma warning(pop)

#endif // WIN32 && _MSC_VER > 1100

#endif // _XPLATWRL_H_