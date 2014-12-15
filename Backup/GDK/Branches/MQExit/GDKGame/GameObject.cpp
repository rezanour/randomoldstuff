#include "StdAfx.h"
#include "ObjectModel\GameObject.h"
#include <CoreServices\PropertyBag.h>
#include <CoreServices\StreamHelper.h>

#include "Screen.h"

using namespace GDK;
using namespace CoreServices;
using namespace DirectX;

static ulong s_nextID = 0;

GameObject::GameObject(_In_ const std::string& name)
    : _id(s_nextID++), _name(name), _transformSalt(0), _spPropertyBag(new PropertyBag)
{
    _transform.Position = XMFLOAT3(0, 0, 0);
    _transform.Orientation = XMFLOAT4(0, 0, 0, 1);
    _transform.Scale = XMFLOAT3(1, 1, 1);
    UpdateMatrix();
}

GameObject::~GameObject()
{
}

HRESULT GameObject::Create(_In_ const std::string& name, _Out_ GameObjectPtr& spGameObject)
{
    HRESULT hr = S_OK;

    spGameObject.attach(new GameObject(name));
    ISNOTNULL(spGameObject, E_OUTOFMEMORY);

EXIT
    return hr;
}

HRESULT GameObject::Load(_In_ GDK::Screen* pScreen, _In_ uint64 id, _Out_ GameObjectPtr& spGameObject)
{
    HRESULT hr = S_OK;

    stde::com_ptr<IStream> spStream;
    ConfigurationPtr spProperties;

    ISNOTNULL(pScreen, E_INVALIDARG);

    FATAL(pScreen->GetContentManager()->GetStream(id, &spStream));
    FATAL(Configuration::Load(spStream, &spProperties));

    {
        FATAL(Create(spProperties->GetStringValue("Name", "untitled"), spGameObject));

        GDK::Transform transform = spProperties->GetValue<GDK::Transform>("Transform", GDK::Transform());
        spGameObject->SetTransform(transform);

        // look for components:
        std::string componentNames[] = { "VisualComponent", "CameraComponent", "LightComponent" };
        for (size_t i = 0; i < _countof(componentNames); ++i)
        {
            stde::ref_counted_ptr<IObjectComponent> spComponent;
            uint64 componentId = spProperties->GetValue<uint64>(componentNames[i], 0);
            if (componentId > 0)
            {
                FATAL(pScreen->GetScene()->CreateComponent(spGameObject, componentId, &spComponent));
            }
        }
    }

EXIT
    if (FAILED(hr))
    {
        spGameObject.reset();
    }
    return hr;
}

// IGameObject
GDK_METHODIMP_(unsigned long) GameObject::GetID() const
{
    return _id;
}

GDK_METHODIMP_(const char* const) GameObject::GetName() const
{
    return _name.c_str();
}

GDK_METHODIMP_(void) GameObject::SetName(_In_ const char* name)
{
    _name = name;
}

GDK_METHODIMP_(size_t) GameObject::GetTransformSalt() const
{
    return _transformSalt;
}

GDK_METHODIMP_(void) GameObject::GetTransform(_Out_ GDK::Transform* pTransform) const
{
    if (pTransform)
    {
        *pTransform = _transform;
    }
}

GDK_METHODIMP_(void) GameObject::SetTransform(_In_ const GDK::Transform& transform)
{
    _transform = transform;
    UpdateMatrix();
    _transformSalt++;
}

GDK_METHODIMP GameObject::GetPropertyBag(_Deref_out_ GDK::IPropertyBag** ppProperties) const
{
    HRESULT hr = S_OK;

    ISNOTNULL(ppProperties, E_POINTER);

    *ppProperties = _spPropertyBag.as<GDK::IPropertyBag>();

EXIT
    return hr;
}

void GameObject::UpdateMatrix()
{
    XMStoreFloat4x4(&_transform.Matrix, GDK::BuildMatrix(_transform.Scale, _transform.Orientation, _transform.Position));
}

