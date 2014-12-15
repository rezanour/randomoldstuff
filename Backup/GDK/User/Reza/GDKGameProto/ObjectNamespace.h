#pragma once

#include "BaseRefCounted.h"
#include "IObjectNamespace.h"

namespace GDK
{
    class ObjectNamespace : public BaseRefCounted<IObjectNamespace>
    {
    public:
        // Construction/Destruction
        static HRESULT Create(ObjectNamespace** ppObjNS);

        // IObjectNamespace
        HRESULT AddObject(IObject* object);
        HRESULT FindObjectByName(const char* name, IObject** ppObject);

    private:
        ObjectNamespace();
        ~ObjectNamespace();
    };
} // GDK
