#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_SCRIPTING_H_
#define _GDK_SCRIPTING_H_

////////////////////////////////////////////////////////////////////////////////
// scripting support (C++ only for now)

namespace GDK
{
    struct IGameObject;

    struct IGameScene : IRefCounted
    {
        // Create instantiates an object, but does not add it to the scene
        GDK_IMETHOD CreateObject(_In_ void** typeInfo, _Out_ IGameObject** object) = 0;

        GDK_IMETHOD AddObject(_In_ IGameObject* object) = 0;

        GDK_IMETHOD RemoveObject(_In_ uint32_t id) = 0;
        GDK_IMETHOD RemoveObject(_In_ IGameObject* object) = 0;

        GDK_IMETHOD FindObject(_In_ uint32_t id, _Out_ IGameObject** object) = 0;
    };

    typedef HRESULT (*ScriptCallback)(_In_ Message* message, _In_opt_ IGameObject* thisObj, _In_ IGameScene* scene);

} // GDK

#endif // _GDK_SCRIPTING_H_
