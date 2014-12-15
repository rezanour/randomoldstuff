#include "StdAfx.h"
#include "ContentTag.h"
#include "StreamHelper.h"
#include "Debug.h"
#include "UniqueId.h"

using namespace CoreServices;
using namespace GDK;

ContentTag::ContentTag(_In_ GDK::ContentType::Enum contentType, _In_ uint32 subContentType, _In_ const std::string& name)
    : _contentType(contentType), _objectType(static_cast<ObjectType::Enum>(subContentType)), _name(name)
{
    _id = CreateUniqueId();
}

ContentTag::ContentTag(_In_ GDK::ContentType::Enum contentType, _In_ uint32 subContentType, _In_ const std::string& name, _In_ uint64 id)
    : _contentType(contentType), _objectType(static_cast<ObjectType::Enum>(subContentType)), _name(name), _id(id)
{
}

HRESULT ContentTag::Load(_In_ stde::com_ptr<IStream>& spStream, _Out_ ContentTagPtr& spContentTag)
{
    HRESULT hr = S_OK;

    uint64 id;
    ContentType::Enum contentType;
    uint32 subContentType;
    ulong nameLength;

    StreamHelper reader(spStream);

    CHECKHR((reader.ReadValue(id)));
    CHECKHR((reader.ReadValue(contentType)));
    CHECKHR((reader.ReadValue(subContentType)));
    CHECKHR((reader.ReadValue(nameLength)));

    {
        std::vector<char> name(nameLength + 1);
        ulong cbRead = 0;

        CHECKHR(spStream->Read(name.data(), nameLength, &cbRead));
        name[nameLength] = '\0';

        spContentTag.attach(new ContentTag(contentType, subContentType, name.data(), id));
    }

EXIT
    if (FAILED(hr))
        spContentTag.reset();

    return hr;
}

HRESULT ContentTag::Save(_In_ stde::com_ptr<IStream>& spStream)
{
    HRESULT hr = S_OK;

    uint32 subContentType = static_cast<uint32>(_objectType);
    ulong cbWritten = 0;

    StreamHelper writer(spStream);

    CHECKHR((writer.WriteValue(_id)));
    CHECKHR((writer.WriteValue(_contentType)));
    CHECKHR((writer.WriteValue(subContentType)));
    CHECKHR((writer.WriteValue(_name.size(), 4)));
    CHECKHR(spStream->Write(_name.c_str(), static_cast<ulong>(_name.size()), &cbWritten));

EXIT

    return hr;
}

// IContentTag
GDK_IMETHODIMP_(ContentType::Enum) ContentTag::GetType() const
{
    return _contentType;
}

GDK_IMETHODIMP_(ObjectType::Enum)     ContentTag::GetObjectType() const
{
    if (_contentType != ContentType::ObjectContent)
        return ObjectType::InvalidObject;

    return _objectType;
}

GDK_IMETHODIMP_(ComponentType::Enum)  ContentTag::GetComponentType() const
{
    if (_contentType != ContentType::ComponentContent)
        return ComponentType::InvalidComponent;

    return _componentType;
}

GDK_IMETHODIMP_(AssetType::Enum)      ContentTag::GetAssetType() const
{
    if (_contentType != ContentType::AssetContent)
        return AssetType::InvalidAsset;

    return _assetType;
}

GDK_IMETHODIMP_(ResourceType::Enum)   ContentTag::GetResourceType() const
{
    if (_contentType != ContentType::ResourceContent)
        return ResourceType::InvalidResource;

    return _resourceType;
}

GDK_IMETHODIMP_(uint64) ContentTag::GetID() const
{
    return _id;
}

GDK_IMETHODIMP_(const char* const) ContentTag::GetName() const
{
    return _name.c_str();
}

