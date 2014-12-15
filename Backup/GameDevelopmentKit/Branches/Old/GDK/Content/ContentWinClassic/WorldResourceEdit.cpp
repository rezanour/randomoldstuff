#include "Precomp.h"

using Microsoft::WRL::ComPtr;

namespace GDK {
namespace Content {

// IPersistResource
HRESULT GDKAPI WorldResourceEdit::Save(_In_ GDK::IStream* output)
{
    if ( !output )
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    ULONG cbWritten = 0;

    // Write header
    hr = output->Write(&_header, sizeof(_header), &cbWritten);
    if (FAILED(hr))
    {
        return hr;
    }

    // Write objects
    for (size_t i = 0; SUCCEEDED(hr) && i < _gameObjects.size(); i++)
    {
        auto gameObject = _gameObjects[i];
        hr = output->Write(&gameObject, sizeof(gameObject), &cbWritten);
    }

    return hr;
}

HRESULT GDKAPI WorldResourceEdit::Load(_In_ GDK::IStream* input)
{    
    if ( !input )
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    ULONG cbRead = 0;

    // Read header
    hr = input->Read(&_header, sizeof(_header), &cbRead);
    if (FAILED(hr))
    {
        return hr;
    }

    if (_header.header.Version != WORLD_RESOURCE_VERSION)
    {
        return E_INVALIDARG;
    }

    // Read objects
    for (size_t i = 0; i < _header.numObjects; i++)
    {
        WORLD_RESOURCE_OBJECT gameObject = {0};
        hr = input->Read(&gameObject, sizeof(gameObject), &cbRead);
        if (SUCCEEDED(hr))
        {
            _gameObjects.push_back(gameObject);
        }
    }

    return hr;
}

HRESULT GDKAPI WorldResourceEdit::SetName(_In_ const wchar_t* name)
{
    if (!name)
    {
        return E_INVALIDARG;
    }

    _resourceName = name;

    return S_OK;
}

// IWorldResourceEdit
HRESULT GDKAPI WorldResourceEdit::CreateWorldResource(_COM_Outptr_ IWorldResource** resource)
{
    UNREFERENCED_PARAMETER(resource);
    return E_NOTIMPL;
}

// ITextureResource
HRESULT GDKAPI WorldResourceEdit::GetName(_Out_ const wchar_t** name)
{
    if (!name)
    {
        return E_INVALIDARG;
    }

    *name = _resourceName.c_str();

    return S_OK;
}

HRESULT GDKAPI WorldResourceEdit::GetWorldName(_Out_ const wchar_t** name)
{
    if (!name)
    {
        return E_INVALIDARG;
    }

    *name = _header.name;

    return S_OK;
}

HRESULT GDKAPI WorldResourceEdit::GetObjects(_Out_ uint32_t* count, _Out_ const WORLD_RESOURCE_OBJECT** objects)
{
    if (!count || !objects)
    {
        return E_INVALIDARG;
    }

    *count = static_cast<uint32_t>(_gameObjects.size());
    *objects = _gameObjects.data();

    return S_OK;
}

WorldResourceEdit::WorldResourceEdit()
{
    ZeroMemory(&_header, sizeof(_header));
}

WorldResourceEdit::~WorldResourceEdit()
{
}

// ITextureResource

} // Content
} // GDK
