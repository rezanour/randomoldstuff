#pragma once

#include "RefCounted.h"

namespace GDK
{
    class TypeInfo;
    class Var;
    class Method;

    class ObjectInstance : public RefCounted<IRefCounted>
    {
    public:
        template <typename Ty>
        static HRESULT CreateFromTypeInfo(_In_ const TypeInfo* typeInfo, _Out_ Ty** instance);

        uint64_t GetID() const;
        uint32_t GetTypeID() const;

        const Var* GetProperty(_In_ uint16_t id) const;
        Var* GetProperty(_In_ uint16_t id);

        HRESULT InvokeMethod(_In_ uint16_t id, _In_ uint16_t numArgs, _In_count_(numArgs) Var** args, _Out_opt_ Var* returnValue);

    protected:
        ObjectInstance();

    private:
        virtual ~ObjectInstance();

        HRESULT InitializeFromTypeInfo(_In_ const TypeInfo* typeInfo);

        uint64_t    _id;
        uint32_t    _typeID;
        uint16_t    _numProperties;
        uint16_t    _numMethods;
        Var*        _properties;
        Method**    _methods;

        static uint64_t s_nextID;
    };
} // GDK

#include "ObjectInstance.inl"
