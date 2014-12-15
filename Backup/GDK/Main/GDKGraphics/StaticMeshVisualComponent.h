#pragma once

#include "BaseVisualComponent.h"
#include "Model.h"

namespace Lucid3D
{
    class StaticMeshVisualComponent : public BaseVisualComponent
    {
    public:
        static HRESULT Create(  _In_ Renderer* pRenderer, 
                                _In_ GDK::IGameObject* pGameObject, 
                                _In_ uint64 contentId,
                                _Out_ BaseVisualPtr& spComponent);

        // IObjectComponent
        GDK_IMETHOD_(const GDK::IGameObject*) GetOwner() const;
        GDK_IMETHOD_(GDK::ObjectComponentType) GetType() const;

        // BaseVisualComponent
        GDK_IMETHOD Process(_Inout_ RenderMap& renderMap);

    private:
        StaticMeshVisualComponent(_In_ Renderer* pRenderer, _In_ GDK::IGameObject* pGameObject, _In_ ModelPtr& spModel);

        Renderer* _pRenderer;
        GDK::IGameObject* _pOwner;
        ModelPtr _spModel;
    };
}

