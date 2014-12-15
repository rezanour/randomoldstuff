#pragma once

#include <GDK\Graphics.h>
#include <vector>
#include <map>

#include "GraphicsTypes.h"
#include "RendererView.h"
#include "BaseVisualComponent.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "RenderTask.h"

namespace Lucid3D
{
    class RendererScene;
    typedef stde::ref_counted_ptr<RendererScene> ScenePtr;

    class RendererScene : stde::non_copyable, public GDK::RefCountedBase<GDK::IRendererScene>
    {
    public:
        RendererScene(_In_ Renderer* pRenderer);

        // IRendererScene
        GDK_METHOD_(bool) IsVisible() const;
        GDK_METHOD_(void) SetVisible(_In_ bool visible);

        GDK_METHOD CreateComponent(_In_ GDK::IGameObject* pGameObject, _In_ uint64 contentId, _Deref_out_ GDK::IObjectComponent** ppComponent);
        GDK_METHOD RemoveComponent(_In_ GDK::IObjectComponent* pComponent);

        GDK_METHOD SetActiveCamera(_In_ GDK::IObjectComponent* pComponent);

        GDK_METHOD AssociateView(_In_ GDK::IRendererView* view);
        GDK_METHOD DisassociateView(_In_ GDK::IRendererView* view);
        GDK_METHOD GetAssociatedViews(_Out_cap_(maxViews) GDK::IRendererView** views, _In_ size_t maxViews, _Out_ size_t* pNumViews);

        // Methods
        HRESULT DrawGeometry(_In_ ContextPtr& spContext);
        HRESULT DrawLights(_In_ ContextPtr& spContext);
        CameraPtr GetActiveCamera() const { return _spActiveCamera; }

    private:
        Renderer* _pRenderer;
        bool _isVisible;

        std::vector<BaseVisualPtr> _visuals;
        std::vector<LightPtr> _lights;
        std::vector<CameraPtr> _cameras;
        CameraPtr _spActiveCamera;

        std::vector<ViewPtr> _views;

        // render tasks for the frame (populated each frame)
        RenderMap _renderMap;
    };
}
