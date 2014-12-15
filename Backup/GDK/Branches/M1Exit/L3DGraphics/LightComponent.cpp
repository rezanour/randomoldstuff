#include "StdAfx.h"
#include "LightComponent.h"
#include "Renderer.h"
#include <CoreServices\Configuration.h>

using namespace Lucid3D;
using namespace GDK;
using namespace CoreServices;

HRESULT LightComponent::Create( _In_ Renderer* pRenderer, 
                                _In_ IGameObject* pGameObject, 
                                _In_ uint64 contentId, 
                                _Out_ LightPtr& spComponent)
{
    HRESULT hr = S_OK;

    stde::com_ptr<IStream> spStream;
    ConfigurationPtr spProperties;
    IContentManagerPtr spContentManager;

    // TODO: It would be nice if all the lights (and all other quads) shared the same
    // geometry
    GeometryPtr spGeometry;

    ISNOTNULL(pRenderer, E_INVALIDARG);
    ISNOTNULL(pGameObject, E_INVALIDARG);

    spContentManager = pRenderer->GetContentManager();
    CHECKHR(spContentManager->GetStream(contentId, &spStream));
    CHECKHR(Configuration::Load(spStream, &spProperties));

    // TODO: Figure out how to read vector properties from the config (Color)
    L3DMath::Vector3 color(0, 0, 1);

    CHECKHR(Geometry::CreateFullscreenQuad(pRenderer, pRenderer->GetContentManager(), spGeometry));
    spComponent.attach(new LightComponent(pRenderer, pGameObject, spGeometry, color));

EXIT
    return hr;
}

LightComponent::LightComponent(_In_ Renderer* pRenderer, _In_ IGameObject* pGameObject, _In_ GeometryPtr& spGeometry, _In_ const L3DMath::Vector3& color)
    : _pRenderer(pRenderer), _pOwner(pGameObject), _spQuad(spGeometry), _color(color)
{
}

// IObjectComponent
GDK_METHODIMP_(const GDK::IGameObject*) LightComponent::GetOwner() const
{
    return _pOwner;
}

GDK_METHODIMP_(GDK::ObjectComponentType) LightComponent::GetType() const
{
    return ObjectComponentType::VisualComponent;
}

GDK_METHODIMP LightComponent::Process(_Inout_ RenderMap& renderMap)
{
    ShaderPtr spShader = _pRenderer->GetShaderForMaterial(0xffffffff);
    MaterialPtr spMaterial;
    Transform transform;
    _pOwner->GetTransform(&transform);
    renderMap[spShader].push_back(std::make_shared<LightRenderTask>(transform.Matrix, _spQuad, spMaterial, spShader, this));
    return S_OK;
}

