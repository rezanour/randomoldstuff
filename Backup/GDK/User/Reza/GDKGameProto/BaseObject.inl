#pragma once

#include "ITypeObject.h"
#include "Executive.h"
#include "MemoryManager.h"

namespace GDK
{
    template <typename I0, typename I1, typename I2>
    inline BaseObject<I0, I1, I2>::BaseObject()
        : _name(nullptr), _type(nullptr)
    {
    }

    template <typename I0, typename I1, typename I2>
    inline BaseObject<I0, I1, I2>::~BaseObject()
    {
        if (_name)
        {
            GDK::Executive::GetMemoryManager()->Free(_name);
            _name = nullptr;
        }

        if (_type)
        {
            _type->Release();
            _type = nullptr;
        }
    }

    template <typename I0, typename I1, typename I2>
    inline const char* BaseObject<I0, I1, I2>::GetName() const
    {
        return _name;
    }

    template <typename I0, typename I1, typename I2>
    inline GDK::ITypeObject* BaseObject<I0, I1, I2>::GetType() const
    {
        return _type;
    }

    template <typename I0, typename I1, typename I2>
    inline void BaseObject<I0, I1, I2>::SetName(const char* name)
    {
        if (_name)
        {
            GDK::Executive::GetMemoryManager()->Free(_name);
            _name = nullptr;
        }

        if (name)
        {
            size_t len = strlen(name) + 1;
#ifndef NDEBUG
            char tag[200] = {0};
            sprintf_s(tag, "Object Name (%s)", name);
            _name = static_cast<char*>(GDK::Executive::GetMemoryManager()->Alloc(len, tag));
#else
            _name = static_cast<char*>(GDK::Executive::GetMemoryManager()->Alloc(len, name));
#endif
            if (_name)
            {
                StringCchCopyA(_name, len, name);
            }
        }
    }

    template <typename I0, typename I1, typename I2>
    inline void BaseObject<I0, I1, I2>::SetType(GDK::ITypeObject* type)
    {
        _type = type;
        _type->AddRef();
    }
} // GDK
