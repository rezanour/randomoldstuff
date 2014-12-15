#include "StdAfx.h"

using namespace GDK;

GDK_METHOD Screen::Create(_In_opt_ const wchar_t* name, _Out_ Screen** screen)
{
    HRESULT hr = S_OK;

    CHECKP(screen, E_POINTER);

    *screen = new Screen;
    CHECKP(*screen, E_OUTOFMEMORY);

    if (name)
    {
        (*screen)->SetName(name);
    }

    CHECK(ObjectFactory::Create(&(*screen)->_factory));
    (*screen)->_maxObjects = 1000;
    (*screen)->_objects = new Object*[(*screen)->_maxObjects];
    CHECKP((*screen)->_objects, E_OUTOFMEMORY);

Exit:
    if (FAILED(hr))
    {
        SafeRelease(*screen);
    }

    return hr;
}

Screen::Screen()
    : _name(nullptr), _factory(nullptr), _numObjects(0), _maxObjects(0), _objects(nullptr)
{
}

Screen::~Screen()
{
    free(_name);
    _factory->Release();

    for (size_t i = 0; i < _numObjects; ++i)
    {
        _objects[i]->Release();
    }

    _numObjects = _maxObjects = 0;
    SafeDeleteArray(_objects);
}

GDK_IMETHODIMP Screen::CreateObject(_In_ void** typeInfo, _Out_ IGameObject** object)
{
    HRESULT hr = S_OK;

    Object* obj = nullptr;
    CHECK(_factory->CreateObject(typeInfo, &obj));
    *object = obj;

Exit:
    if (FAILED(hr))
    {
        *object = nullptr;
    }
    return hr;
}

GDK_IMETHODIMP Screen::AddObject(_In_ IGameObject* object)
{
    HRESULT hr = S_OK;
    CHECKP(object, E_INVALIDARG);
    CHECKP(_objects, E_UNEXPECTED);
    CHECKB(_numObjects < _maxObjects, E_OUTOFMEMORY);

    _objects[_numObjects] = dynamic_cast<Object*>(object);
    _objects[_numObjects++]->AddRef();

Exit:
    return hr;
}

GDK_IMETHODIMP Screen::RemoveObject(_In_ uint32_t id)
{
    for (size_t i = 0; i < _numObjects; ++i)
    {
        if (_objects[i]->GetID() == id)
        {
            _objects[i]->Release();
            for (size_t j = i, count = --_numObjects; j < count; ++j)
            {
                _objects[j] = _objects[j + 1];
            }
            return S_OK;
        }
    }

    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

GDK_IMETHODIMP Screen::RemoveObject(_In_ IGameObject* object)
{
    for (size_t i = 0; i < _numObjects; ++i)
    {
        if (static_cast<IGameObject*>(_objects[i]) == object)
        {
            _objects[i]->Release();
            for (size_t j = i, count = --_numObjects; j < count; ++j)
            {
                _objects[j] = _objects[j + 1];
            }
            return S_OK;
        }
    }

    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

GDK_IMETHODIMP Screen::FindObject(_In_ uint32_t id, _Out_ IGameObject** object)
{
    HRESULT hr = S_OK;

    CHECKP(object, E_POINTER);

    size_t i = 0;
    for (; i < _numObjects; ++i)
    {
        if (_objects[i]->GetID() == id)
        {
            _objects[i]->AddRef();
            *object = _objects[i];
            hr = S_OK;
            break;
        }
    }

    if (i == _numObjects)
    {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

Exit:
    return hr;
}

GDK_METHOD Screen::SetName(_In_ const wchar_t* name)
{
    _name = _wcsdup(name);
    return S_OK;
}

GDK_METHOD_(const wchar_t*) Screen::GetName() const
{
    return _name;
}
