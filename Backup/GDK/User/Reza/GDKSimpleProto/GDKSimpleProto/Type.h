#pragma once

#include "Object.h"
#include "IType.h"

namespace GDK
{
    class Type : public Object<IType>
    {
    public:
        // Creation
        static HRESULT Create(const char* name, Type** type);

        // The root type of all types is 'Type'
        static IType* GetRootType();

        // IType

    private:
        Type(const char* name);
        virtual ~Type();

        static Type* s_rootType;
    };
} // GDK
