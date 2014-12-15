#pragma once

namespace GDK
{
    template <typename Ty>
    inline HRESULT ObjectInstance::CreateFromTypeInfo(_In_ const TypeInfo* typeInfo, _Out_ Ty** instance)
    {
        static_assert(std::is_base_of<ObjectInstance, Ty>::value, "template parameter must derive from ObjectInstance");

        if (!instance)
            return E_POINTER;

        if (!typeInfo)
            return E_INVALIDARG;

        *instance = nullptr;
        Ty* object = new Ty();
        HRESULT hr = object ? S_OK : E_OUTOFMEMORY;

        if (SUCCEEDED(hr))
        {
            hr = object->InitializeFromTypeInfo(typeInfo);
        }

        if (SUCCEEDED(hr))
        {
            *instance = object;
        }
        else if (object)
        {
            object->Release();
        }

        return hr;
    }
} // GDK
