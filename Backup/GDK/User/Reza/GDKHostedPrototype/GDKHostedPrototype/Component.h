#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_COMPONENT_H_
#define _GDK_COMPONENT_H_

////////////////////////////////////////////////////////////////////////////////
// implementation of base component

namespace GDK
{
    enum ComponentType
    {
        ComponentType_Camera,
        ComponentType_Light,
        ComponentType_Mesh,
        ComponentType_Dynamics,
        ComponentType_Collision,
        ComponentType_AudioEmitter,
        ComponentType_AudioListener,
    };

    class Component : public RefCounted<>
    {
    public:
        ComponentType GDK_API GetType() const;
        Object* GDK_API GetOwner() const;

    protected:
        Component(_In_ Object* owner, _In_ ComponentType type);
        virtual ~Component();

    private:
        const ComponentType _type;
        Object* _owner;
    };
} // GDK

#endif // _GDK_COMPONENT_H_
