#pragma once

#include "RefCounted.h"
#include "IObject.h"

namespace GDK
{
    namespace _internal2_
    {
        struct IEmpty {};
        struct IEmpty2 {};
        struct IEmpty3 {};
    };

    template <typename I0 = _internal2_::IEmpty, typename I1 = _internal2_::IEmpty2, typename I2 = _internal2_::IEmpty3>
    class Object : public RefCounted<IObject, I0, I1, I2>
    {
    public:
        // Creation
        static HRESULT Create(Object** object);

        // IObject
        uint32_t GetID() const;
        IType* GetType() const;
        const char* GetName() const;

        // Methods
        void SetName(const char* name);

    protected:
        Object();
        virtual ~Object();

        void SetType(IType* type);

    private:
        uint32_t _id;
        IType* _type;
        char* _name;

        static uint32_t s_nextID;
    };
} // GDK

#include "Object.inl"
