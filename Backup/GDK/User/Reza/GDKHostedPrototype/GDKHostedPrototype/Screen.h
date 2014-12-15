#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_SCREEN_H_
#define _GDK_SCREEN_H_

////////////////////////////////////////////////////////////////////////////////
// screen

namespace GDK
{
    class Object;
    class ObjectFactory;

    class Screen : public RefCounted<IGameScene>
    {
    public:
        // Creation
        static GDK_METHOD Create(_In_opt_ const wchar_t* name, _Out_ Screen** screen);

        // IGameScene
        GDK_IMETHOD CreateObject(_In_ void** typeInfo, _Out_ IGameObject** object);
        GDK_IMETHOD AddObject(_In_ IGameObject* object);
        GDK_IMETHOD RemoveObject(_In_ uint32_t id);
        GDK_IMETHOD RemoveObject(_In_ IGameObject* object);
        GDK_IMETHOD FindObject(_In_ uint32_t id, _Out_ IGameObject** object);

        // Name
        GDK_METHOD SetName(_In_ const wchar_t* name);
        GDK_METHOD_(const wchar_t*) GetName() const;

        // Messages

    private:
        Screen();
        ~Screen();

        wchar_t* _name;
        ObjectFactory* _factory;
        uint32_t _numObjects;
        uint32_t _maxObjects;
        Object** _objects;
    };

} // GDK

#endif // _GDK_SCREEN_H_
