#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_OBJECTFACTORY_H_
#define _GDK_OBJECTFACTORY_H_

////////////////////////////////////////////////////////////////////////////////
// implementation of game object factory, which uses object type templates to create instances

namespace GDK
{
    class Object;

    class ObjectFactory : public RefCounted<>
    {
    public:
        static GDK_METHOD Create(_Out_ ObjectFactory** factory);

        GDK_METHOD CreateObject(_In_ void* typeInfo, _Out_ Object** object);

    private:
        ObjectFactory();
        ~ObjectFactory();
    };
} // GDK

#endif // _GDK_OBJECTFACTORY_H_
