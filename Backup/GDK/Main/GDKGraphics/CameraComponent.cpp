#include "StdAfx.h"
#include "CameraComponent.h"
#include "Renderer.h"
#include <CoreServices\Configuration.h>

using namespace Lucid3D;
using namespace GDK;
using namespace CoreServices;

HRESULT CameraComponent::Create(_In_ Renderer* pRenderer,
                                _In_ IGameObject* pGameObject, 
                                _In_ uint64 contentId, 
                                _Out_ CameraPtr& spComponent)
{
    HRESULT hr = S_OK;

    stde::com_ptr<IStream> spStream;
    ConfigurationPtr spProperties;
    IContentManagerPtr spContentManager;

    ISNOTNULL(pRenderer, E_INVALIDARG);
    ISNOTNULL(pGameObject, E_INVALIDARG);

    spContentManager = pRenderer->GetContentManager();
    CHECKHR(spContentManager->GetStream(contentId, &spStream));
    CHECKHR(Configuration::Load(spStream, &spProperties));

    float fov = spProperties->GetValue<float>("Fov", 60.0f);
    float nearClip = spProperties->GetValue<float>("NearClip", 0.1f);
    float farClip = spProperties->GetValue<float>("FarClip", 1000.0f);

    float aspectRatio = (float)pRenderer->GetSettings().DefaultView.ScreenWidth / 
                        (float)pRenderer->GetSettings().DefaultView.ScreenHeight;

    // NOTE: Only support perspective camera right now
    spComponent.attach(new CameraComponent(pGameObject, fov, aspectRatio, nearClip, farClip));

EXIT
    return hr;
}

CameraComponent::CameraComponent(_In_ IGameObject* pGameObject, _In_ float fov, _In_ float aspectRatio, _In_ float nearClip, _In_ float farClip)
    : _pOwner(pGameObject), _fov(fov), _aspectRatio(1.0f / aspectRatio), _nearClipPlane(nearClip), _farClipPlane(farClip)
{
    _projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), aspectRatio, nearClip, farClip);
}

// IObjectComponent
GDK_IMETHODIMP_(const GDK::IGameObject*) CameraComponent::GetOwner() const
{
    return _pOwner;
}

GDK_IMETHODIMP_(GDK::ObjectComponentType) CameraComponent::GetType() const
{
    return ObjectComponentType::CameraComponent;
}

void CameraComponent::UpdateMatrices()
{
    Transform transform;
    GetOwner()->GetTransform(&transform);
    _transformSalt = GetOwner()->GetTransformSalt();

    _view = DirectX::XMMatrixLookToLH(XMLoadFloat3(&transform.Position), GetForward(XMLoadFloat4x4(&transform.Matrix)), GetUp(XMLoadFloat4x4(&transform.Matrix)));
}
