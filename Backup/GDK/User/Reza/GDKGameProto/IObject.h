#pragma once

#include "IRefCounted.h"

namespace GDK
{
    __interface ITypeObject;

    __interface IObject : public IRefCounted
    {
        const char* GetName() const;
        ITypeObject* GetType() const;
    };
} // GDK
