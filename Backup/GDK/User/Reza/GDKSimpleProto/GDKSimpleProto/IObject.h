#pragma once

#include "IRefCounted.h"

namespace GDK
{
    __interface IType;

    __interface IObject : public IRefCounted
    {
        uint32_t GetID() const;
        const IType* GetType() const;
        const char* GetName() const;
    };
} // GDK
