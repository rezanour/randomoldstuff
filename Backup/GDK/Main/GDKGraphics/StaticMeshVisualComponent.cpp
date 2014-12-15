#include "StdAfx.h"
#include "StaticMeshVisualComponent.h"
#include <CoreServices\Configuration.h>

using namespace Lucid3D;
using namespace GDK;
using namespace CoreServices;

HRESULT StaticMeshVisualComponent::Create(  _In_ Renderer* pRenderer, 
                                            _In_ IGameObject* pGameObject, 
                                            _In_ uint64 contentId, 
                                            _Out_ BaseVisualPtr& spComponent)
{
    HRESULT hr = S_OK;

    stde::com_ptr<IStream> spStream;
    ConfigurationPtr spProperties;
    IContentManagerPtr spContentManager;
    ModelPtr spModel;

    ISNOTNULL(pRenderer, E_INVALIDARG);
    ISNOTNULL(pGameObject, E_INVALIDARG);

    spContentManager = pRenderer->GetContentManager();
    CHECKHR(spContentManager->GetStream(contentId, &spStream));
    CHECKHR(Configuration::Load(spStream, &spProperties));

    uint64 modelId = spProperties->GetValue<uint64>("Model", 0);
    CHECKHR(Model::Create(pRenderer, spContentManager, modelId, spModel));
    spComponent.attach(new StaticMeshVisualComponent(pRenderer, pGameObject, spModel));

EXIT
    return hr;
}

StaticMeshVisualComponent::StaticMeshVisualComponent(_In_ Renderer* pRenderer, _In_ IGameObject* pGameObject, _In_ ModelPtr& spModel)
    : _pRenderer(pRenderer), _pOwner(pGameObject), _spModel(spModel)
{
}

// IObjectComponent
GDK_IMETHODIMP_(const GDK::IGameObject*) StaticMeshVisualComponent::GetOwner() const
{
    return _pOwner;
}

GDK_IMETHODIMP_(GDK::ObjectComponentType) StaticMeshVisualComponent::GetType() const
{
    return ObjectComponentType::VisualComponent;
}

GDK_IMETHODIMP StaticMeshVisualComponent::Process(_Inout_ RenderMap& renderMap)
{
    Transform transform;
    GetOwner()->GetTransform(&transform);
    return _spModel->Process(renderMap, transform);
}

