#pragma once

#include "IRefCounted.h"

namespace GDK
{
    __interface ITypeObject : public IRefCounted
    {
        const char* GetName() const;
        uint32_t GetID() const;
    };
} // GDK
