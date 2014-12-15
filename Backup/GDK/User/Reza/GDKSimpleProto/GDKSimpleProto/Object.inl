#pragma once

#include "Type.h"

namespace GDK
{
    template <typename I0, typename I1, typename I2>
    uint32_t Object<I0, I1, I2>::s_nextID = 0;

    template <typename I0, typename I1, typename I2>
    inline HRESULT Object<I0, I1, I2>::Create(Object** object)
    {
        if (!object)
            return E_POINTER;

        HRESULT hr = S_OK;

        *object = nullptr;

        Object* obj = new Object;
        hr = obj ? S_OK : E_OUTOFMEMORY;

        Type* type = nullptr;
        if (SUCCEEDED(hr))
        {
            hr = Type::Create("Object", &type);
        }

        if (SUCCEEDED(hr))
        {
            obj->SetType(type);
            *object = obj;
        }
        else if (obj)
        {
            obj->Release();
        }

        return hr;
    }

    template <typename I0, typename I1, typename I2>
    inline Object<I0, I1, I2>::Object()
        : _id(s_nextID++), _type(nullptr), _name(nullptr)
    {
    }

    template <typename I0, typename I1, typename I2>
    inline Object<I0, I1, I2>::~Object()
    {
        if (_name)
        {
            delete [] _name;
            _name = nullptr;
        }

        SafeRelease(_type);
    }

    template <typename I0, typename I1, typename I2>
    inline uint32_t Object<I0, I1, I2>::GetID() const
    {
        return _id;
    }

    template <typename I0, typename I1, typename I2>
    inline const char* Object<I0, I1, I2>::GetName() const
    {
        return _name;
    }

    template <typename I0, typename I1, typename I2>
    inline IType* Object<I0, I1, I2>::GetType() const
    {
        return _type;
    }

    template <typename I0, typename I1, typename I2>
    inline void Object<I0, I1, I2>::SetName(const char* name)
    {
        if (_name)
        {
            delete [] _name;
            _name = nullptr;
        }

        if (name)
        {
            size_t len = strlen(name) + 1;
            _name = new char[len];
            if (_name)
            {
                StringCchCopyA(_name, len, name);
            }
        }
    }

    template <typename I0, typename I1, typename I2>
    inline void Object<I0, I1, I2>::SetType(IType* type)
    {
        SafeRelease(_type);
        _type = type;
        _type->AddRef();
    }
} // GDK
