#include "StdAfx.h"

using namespace GDK;

uint32_t Object::s_nextID = 10;

GDK_METHOD Object::Create(_Out_ Object** object)
{
    HRESULT hr = S_OK;

    CHECKP(object, E_POINTER);
    *object = new Object;
    CHECKP(*object, E_OUTOFMEMORY);

Exit:
    return hr;
}

Object::Object()
    : _id(s_nextID++), _numComponents(0), _maxComponents(0), _components(nullptr)
{
}

Object::~Object()
{
    for (uint16_t i = 0; i < _numComponents; ++i)
    {
        _components[i]->Release();
    }
    _numComponents = _maxComponents = 0;
    SafeDeleteArray(_components);
}

GDK_IMETHODIMP_(uint32_t) Object::GetID() const
{
    return _id;
}

GDK_IMETHODIMP_(uint16_t) Object::GetNumProperties() const
{
    return 0;
}

GDK_IMETHODIMP Object::GetProperty(_In_ uint16_t index, _Out_ void** placeHolder)
{
    HRESULT hr = S_OK;

    CHECKP(placeHolder, E_POINTER);
    CHECKB(index == 0, E_INVALIDARG);

    *placeHolder = nullptr;

Exit:
    return hr;
}

uint16_t GDK_API Object::GetNumComponents() const
{
    return _numComponents;
}

Component* GDK_API Object::GetComponent(_In_ uint16_t index) const
{
    if (index < _numComponents)
    {
        return _components[index];
    }

    return nullptr;
}

GDK_METHOD Object::AddComponent(_In_ Component* component)
{
    HRESULT hr = S_OK;
    CHECKP(component, E_INVALIDARG);
    CHECKB(_numComponents < _maxComponents, E_OUTOFMEMORY);

    uint16_t i = 0;
    for (; i < _numComponents; ++i)
    {
        if (_components[i]->GetType() == component->GetType())
        {
            hr = HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
            break;
        }
    }

    if (i == _numComponents)
    {
        _components[_numComponents] = component;
        _components[_numComponents++]->AddRef();
    }

Exit:
    return hr;
}

GDK_METHOD Object::RemoveComponent(_In_ ComponentType type)
{
    HRESULT hr = S_OK;
    CHECKB(_numComponents > 0, HRESULT_FROM_WIN32(ERROR_NOT_FOUND));

    uint16_t i = 0;
    for (; i < _numComponents; ++i)
    {
        if (_components[i]->GetType() == type)
        {
            _components[i]->Release();
            for (uint16_t j = i, count = --_numComponents; j < count; ++j)
            {
                _components[j] = _components[j + 1];
            }
            break;
        }
    }

    if (i == _numComponents)
    {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

Exit:
    return hr;
}

GDK_METHOD Object::RemoveComponent(_In_ Component* component)
{
    HRESULT hr = S_OK;
    CHECKP(component, E_INVALIDARG);
    CHECKB(_numComponents > 0, HRESULT_FROM_WIN32(ERROR_NOT_FOUND));

    uint16_t i = 0;
    for (; i < _numComponents; ++i)
    {
        if (_components[i] == component)
        {
            _components[i]->Release();
            for (uint16_t j = i, count = --_numComponents; j < count; ++j)
            {
                _components[j] = _components[j + 1];
            }
            break;
        }
    }

    if (i == _numComponents)
    {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

Exit:
    return hr;
}

HRESULT OnSoldierCollision(_In_ Message* message, _In_ IGameObject* thisObj, _In_ IGameScene* scene);

GDK_METHOD Object::ProcessMessage(_In_ Message* message)
{
    static ScriptCallback handler = OnSoldierCollision;
    handler(message, this, nullptr);
    return S_OK;
}

HRESULT OnSoldierCollision(_In_ Message* message, _In_ IGameObject* thisObj, _In_ IGameScene* scene)
{
    HRESULT hr = S_OK;

    CHECKP(message, E_INVALIDARG);
    CHECKP(thisObj, E_INVALIDARG);

    UNREFERENCED_PARAMETER(scene);

    uint16_t healthIndex = 0;
    void* foo;
    CHECK(thisObj->GetProperty(healthIndex, &foo));

    // do something with foo

Exit:
    return hr;
}
