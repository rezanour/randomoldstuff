#include "StdAfx.h"
#include "PropertyBag.h"
#include "StreamHelper.h"
#include "Property.h"
#include "Debug.h"

using namespace CoreServices;
using namespace GDK;

PropertyBag::PropertyBag()
{
}

PropertyBag::~PropertyBag()
{
}

// Save/Load
HRESULT PropertyBag::Load(_In_ stde::com_ptr<IStream> spStream, _Out_ PropertyBagPtr& spPropertyBag)
{
    HRESULT hr = S_OK;

    ISNOTNULL(spStream, E_INVALIDARG);

    {
        spPropertyBag.attach(new PropertyBag);
        ulong cbRead = 0;
        size_t numProperties = 0;
        StreamHelper reader(spStream);

        CHECKHR(reader.ReadValue(numProperties, 4));

        for (size_t i = 0; i < numProperties; i++)
        {
            // length prefixed name
            size_t nameLength = 0;
            std::vector<char> name;
            PropertyType type;
            stde::ref_counted_ptr<IProperty> spProp;

            CHECKHR(reader.ReadValue(nameLength, 4));
            name.resize(nameLength + 1);
            CHECKHR(spStream->Read(name.data(), static_cast<ulong>(nameLength), &cbRead));
            name[nameLength] = '\0';

            // type, which then dictates how to parse value
            CHECKHR(reader.ReadValue(type));

            CHECKHR(spPropertyBag->CreateProperty(type, name.data(), &spProp));

            switch (type)
            {
            case PropertyType::Uint64:
                {
                    uint64 value;
                    CHECKHR(reader.ReadValue(value));
                    CHECKHR(spProp->SetUint64(value));
                }
                break;
            case PropertyType::Integer:
                {
                    int value;
                    CHECKHR(reader.ReadValue(value));
                    CHECKHR(spProp->SetInt(value));
                }
                break;
            case PropertyType::Float:
                {
                    float value;
                    CHECKHR(reader.ReadValue(value));
                    CHECKHR(spProp->SetFloat(value));
                }
                break;
            case PropertyType::Boolean:
                {
                    bool value;
                    CHECKHR(reader.ReadValue(value));
                    CHECKHR(spProp->SetBool(value));
                }
                break;
            case PropertyType::String:
                {
                    size_t length;
                    std::vector<char> value;
                    CHECKHR(reader.ReadValue(length, 4));
                    value.resize(length + 1);
                    CHECKHR(spStream->Read(value.data(), static_cast<ulong>(length), &cbRead));
                    CHECKHR(spProp->SetString(value.data()));
                }
                break;
            }
        }
    }

EXIT
    if (FAILED(hr))
    {
        spPropertyBag.reset();
    }
    return hr;
}

HRESULT PropertyBag::Save(_In_ stde::com_ptr<IStream> spStream)
{
    HRESULT hr = S_OK;

    ISNOTNULL(spStream, E_INVALIDARG);

    {
        PropertyMap::iterator it;
        ulong cbWritten = 0;
        StreamHelper writer(spStream);

        CHECKHR(writer.WriteValue(_properties.size(), 4));

        for (it = _properties.begin(); it != _properties.end(); it++)
        {
            // length prefixed name
            CHECKHR(writer.WriteValue(it->first.size(), 4));
            CHECKHR(spStream->Write(it->first.c_str(), (ULONG)it->first.size(), &cbWritten));

            // type, which then dictates how to parse value
            CHECKHR(writer.WriteValue(it->second->GetType()));

            switch (it->second->GetType())
            {
            case PropertyType::Uint64:
                {
                    uint64 value;
                    CHECKHR(it->second->AsUint64(&value));
                    CHECKHR(writer.WriteValue(value));
                }
                break;
            case PropertyType::Integer:
                {
                    int value;
                    CHECKHR(it->second->AsInt(&value));
                    CHECKHR(writer.WriteValue(value));
                }
                break;
            case PropertyType::Float:
                {
                    float value;
                    CHECKHR(it->second->AsFloat(&value));
                    CHECKHR(writer.WriteValue(value));
                }
                break;
            case PropertyType::Boolean:
                {
                    bool value;
                    CHECKHR(it->second->AsBool(&value));
                    CHECKHR(writer.WriteValue(value));
                }
                break;
            case PropertyType::String:
                {
                    char value[1000];
                    CHECKHR(it->second->AsString(value, _countof(value)));
                    CHECKHR(writer.WriteValue(strlen(value), 4));
                    CHECKHR(spStream->Write(value, static_cast<ulong>(strlen(value)), &cbWritten));
                }
                break;
            }
        }
    }

EXIT
    return hr;
}

// IPropertyBag
GDK_METHODIMP_(size_t) PropertyBag::GetCount() const
{
    return _properties.size();
}

GDK_METHODIMP PropertyBag::CreateProperty(_In_ PropertyType type, _In_ const char* name, _Deref_out_ IProperty** ppProperty)
{
    HRESULT hr = S_OK;

    ISNOTNULL(name, E_INVALIDARG);
    ISNOTNULL(ppProperty, E_POINTER);

    {
        PropertyMap::iterator it = _properties.find(name);
        ISTRUE(it == _properties.end(), HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS));

        {
            stde::ref_counted_ptr<Property> spProp(new Property(name, type));
            ISNOTNULL(spProp, E_OUTOFMEMORY);

            _properties[name] = spProp;

            *ppProperty = spProp.detach();
        }
    }

EXIT
    return hr;
}

GDK_METHODIMP PropertyBag::GetProperty(_In_ const char* name, _Deref_out_ GDK::IProperty** ppProperty) const
{
    HRESULT hr = S_OK;

    ISNOTNULL(name, E_INVALIDARG);
    ISNOTNULL(ppProperty, E_POINTER);

    {
        PropertyMap::const_iterator it = _properties.find(name);
        ISTRUE(it != _properties.end(), HRESULT_FROM_WIN32(ERROR_NOT_FOUND));

        *ppProperty = it->second.as<IProperty>();
    }

EXIT
    return hr;
}

GDK_METHODIMP PropertyBag::GetProperty(_In_ size_t index, _Deref_out_ GDK::IProperty** ppProperty) const
{
    HRESULT hr = S_OK;

    ISTRUE(index < _properties.size(), E_INVALIDARG);
    ISNOTNULL(ppProperty, E_POINTER);

    {
        PropertyMap::const_iterator it = _properties.begin();
        for (size_t i = 0; i < index; i++)
            it++;

        *ppProperty = it->second.as<IProperty>();
    }

EXIT
    return hr;
}

GDK_METHODIMP PropertyBag::DeleteProperty(_In_ const char* name)
{
    HRESULT hr = S_OK;

    ISNOTNULL(name, E_INVALIDARG);

    {
        PropertyMap::iterator it = _properties.find(name);
        ISTRUE(it != _properties.end(), HRESULT_FROM_WIN32(ERROR_NOT_FOUND));

        _properties.erase(it);
    }

EXIT
    return hr;
}

GDK_METHODIMP_(void) PropertyBag::Clear()
{
    _properties.clear();
}
