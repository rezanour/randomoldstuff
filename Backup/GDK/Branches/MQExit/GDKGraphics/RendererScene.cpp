#include "StdAfx.h"
#include "RendererScene.h"
#include "StaticMeshVisualComponent.h"
#include <CoreServices\Configuration.h>
#include "CameraComponent.h"

using namespace Lucid3D;
using namespace GDK;
using namespace CoreServices;

RendererScene::RendererScene(_In_ Renderer* pRenderer)
    : _pRenderer(pRenderer), _isVisible(true)
{
}

GDK_METHODIMP_(bool) RendererScene::IsVisible() const
{
    return _isVisible;
}

GDK_METHODIMP_(void) RendererScene::SetVisible(_In_ bool visible)
{
    _isVisible = visible;
}

GDK_METHODIMP RendererScene::CreateComponent(_In_ GDK::IGameObject* pGameObject, _In_ uint64 contentId, _Deref_out_ GDK::IObjectComponent** ppComponent)
{
    HRESULT hr = S_OK;

    IContentTagPtr spContentTag;

    ISNOTNULL(pGameObject, E_INVALIDARG);
    ISNOTNULL(ppComponent, E_POINTER);

    *ppComponent = nullptr;

    // Figure out which type it is { visual, camera, light, etc... }
    CHECKHR(_pRenderer->GetContentManager()->GetContentTag(contentId, &spContentTag));

    // Ensure this is even a component
    ISTRUE(spContentTag->GetType() == ContentType::ComponentContent, E_INVALIDARG);

    switch (spContentTag->GetComponentType())
    {
    case ComponentType::VisualComponent:
        {
            BaseVisualPtr spComponent;
            CHECKHR(StaticMeshVisualComponent::Create(_pRenderer, pGameObject, contentId, spComponent));
            _visuals.push_back(spComponent);
            *ppComponent = spComponent.detach();
        }
        break;

    case ComponentType::CameraComponent:
        {
            CameraPtr spComponent;
            CHECKHR(CameraComponent::Create(_pRenderer, pGameObject, contentId, spComponent));

            _cameras.push_back(spComponent);

            if (!_spActiveCamera)
            {
                _spActiveCamera = spComponent;
            }

            *ppComponent = spComponent.detach();
        }
        break;

    case ComponentType::LightComponent:
        {
            LightPtr spComponent;
            CHECKHR(LightComponent::Create(_pRenderer, pGameObject, contentId, spComponent));
            _lights.push_back(spComponent);
            *ppComponent = spComponent.detach();
        }
        break;

    default:
        CHECKHR(E_INVALIDARG);
    }

EXIT
    return hr;
}

GDK_METHODIMP RendererScene::RemoveComponent(_In_ GDK::IObjectComponent* pComponent)
{
    HRESULT hr = E_NOTIMPL;

EXIT
    return hr;
}

GDK_METHODIMP RendererScene::SetActiveCamera(_In_ IObjectComponent* pComponent)
{
    if (!pComponent)
        return E_INVALIDARG;

    if (pComponent->GetType() != ObjectComponentType::CameraComponent)
    {
        return E_INVALIDARG;
    }

    _spActiveCamera.reset(reinterpret_cast<CameraComponent*>(pComponent));

    return S_OK;
}

GDK_METHODIMP RendererScene::AssociateView(_In_ IRendererView* view)
{
    stde::ref_counted_ptr<RendererView> spView;

    spView.reset(static_cast<RendererView*>(view));

    for (size_t i = 0; i < _views.size(); i++)
    {
        if (_views[i].get() == spView.get())
        {
            // already have the view
            return S_OK;
        }
    }

    _views.push_back(spView);
    return S_OK;
}

GDK_METHODIMP RendererScene::DisassociateView(_In_ IRendererView* view)
{
    stde::ref_counted_ptr<RendererView> spView;

    spView.reset(static_cast<RendererView*>(view));

    for (size_t i = 0; i < _views.size(); i++)
    {
        if (_views[i].get() == spView.get())
        {
            // found it, so remove it
            _views.erase(_views.begin() + i);
            break;
        }
    }

    return S_OK;
}

GDK_METHODIMP RendererScene::GetAssociatedViews(_Out_cap_(maxViews) IRendererView** views, _In_ size_t maxViews, _Out_ size_t* pNumViews)
{
    if (!views || !pNumViews)
    {
        return E_INVALIDARG;
    }

    *pNumViews = min(maxViews, _views.size());

    for (size_t i = 0; i < *pNumViews; i++)
    {
        views[i] = _views[i];
    }

    return S_OK;
}

HRESULT RendererScene::DrawGeometry(_In_ ContextPtr& spContext)
{
    HRESULT hr = S_OK;

    _renderMap.clear();

    for (size_t i = 0; i < _visuals.size(); i++)
    {
        _visuals[i]->Process(_renderMap);
    }

    for (RenderMap::iterator it = _renderMap.begin(); it != _renderMap.end(); it++)
    {
        ShaderPtr shader = it->first;
        shader->RenderBatch(spContext, _spActiveCamera, it->second);
    }

EXIT
    return hr;
}

HRESULT RendererScene::DrawLights(_In_ ContextPtr& spContext)
{
    HRESULT hr = S_OK;

    _renderMap.clear();

    for (size_t i = 0; i < _lights.size(); i++)
    {
        _lights[i]->Process(_renderMap);
    }

    for (RenderMap::iterator it = _renderMap.begin(); it != _renderMap.end(); it++)
    {
        ShaderPtr shader = it->first;
        shader->RenderBatch(spContext, _spActiveCamera, it->second);
    }

EXIT
    return hr;
}
