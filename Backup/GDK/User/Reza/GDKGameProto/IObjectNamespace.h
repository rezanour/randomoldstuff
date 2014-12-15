#pragma once

#include "IRefCounted.h"

namespace GDK
{
    __interface IObject;

    __interface IObjectNamespace : public IRefCounted
    {
        HRESULT AddObject(IObject* object);
        HRESULT FindObjectByName(const char* name, IObject** ppObject);
    };
} // GDK
