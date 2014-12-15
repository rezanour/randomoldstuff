#pragma once

#include "BaseRefCounted.h"
#include "ITypeObject.h"

namespace GDK
{
    class TypeObject : public BaseRefCounted<ITypeObject>
    {
    public:
        static HRESULT Create(const char* name, TypeObject** ppType);

        // ITypeObject
        const char* GetName() const;
        uint32_t GetID() const;

    protected:
        TypeObject();
        ~TypeObject();

    private:
        char*           _name;
        uint32_t        _id;
        static uint32_t s_nextID;
    };
} // GDK
