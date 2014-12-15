#pragma once

#include "BaseRefCounted.h"
#include "IObject.h"

namespace GDK
{
    namespace _internal2_
    {
        struct IEmpty {};
        struct IEmpty2 {};
    };

    template <typename I0, typename I1 = _internal2_::IEmpty, typename I2 = _internal2_::IEmpty2>
    class BaseObject : public BaseRefCounted<IObject, I0, I1, I2>
    {
    public:
        // IObject
        virtual const char* GetName() const;
        virtual ITypeObject* GetType() const;

        // Methods
        void SetName(const char* name);

    protected:
        BaseObject();
        virtual ~BaseObject();

        void SetType(ITypeObject* type);

    private:
        char*           _name;
        ITypeObject*    _type;
    };
} // GDK

#include "BaseObject.inl"
