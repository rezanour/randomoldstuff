#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_OBJECT_H_
#define _GDK_OBJECT_H_

////////////////////////////////////////////////////////////////////////////////
// implementation of game object

namespace GDK
{
    class Component;
    enum ComponentType;
    class ObjectFactory;

    class Object : public RefCounted<IGameObject>
    {
    public:
        static GDK_METHOD Create(_Out_ Object** object);

        // IGameObject
        GDK_IMETHOD_(uint32_t) GetID() const;
        GDK_IMETHOD_(uint16_t) GetNumProperties() const;
        GDK_IMETHOD GetProperty(_In_ uint16_t index, _Out_ void** placeHolder);

        // Components
        uint16_t GDK_API GetNumComponents() const;
        Component* GDK_API GetComponent(_In_ uint16_t) const;
        GDK_METHOD AddComponent(_In_ Component* component);
        GDK_METHOD RemoveComponent(_In_ ComponentType type);
        GDK_METHOD RemoveComponent(_In_ Component* component);

        // Handle messages
        GDK_METHOD ProcessMessage(_In_ Message* message);

    private:
        Object();
        ~Object();

        uint32_t _id;
        uint16_t _numComponents;
        uint16_t _maxComponents;
        Component** _components;

        // Give the object factory private access
        friend class ObjectFactory;

        static uint32_t s_nextID;
    };
} // GDK

#endif // _GDK_OBJECT_H_
