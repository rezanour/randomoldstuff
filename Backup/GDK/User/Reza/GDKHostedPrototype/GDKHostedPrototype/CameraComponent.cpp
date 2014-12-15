#include "StdAfx.h"

using namespace GDK;

HRESULT CameraComponent::Create(_In_ Object* owner, _In_ CameraType type, _Out_ CameraComponent** camera)
{
    HRESULT hr = S_OK;

    CHECKP(owner, E_INVALIDARG);
    CHECKP(camera, E_POINTER);

    *camera = new CameraComponent(owner, type);
    CHECKP(*camera, E_OUTOFMEMORY);

Exit:
    return hr;
}

CameraComponent::CameraComponent(_In_ Object* owner, _In_ CameraType type)
    :  Component(owner, ComponentType_Camera), _type(type)
{
}

CameraComponent::~CameraComponent()
{
}
