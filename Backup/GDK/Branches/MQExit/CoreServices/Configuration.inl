// Implementation of Configuration's template methods
#pragma once

#include <GDK\Transform.h>
#include <type_traits>
#include <stde\conversion.h>

namespace CoreServices
{
    template <typename T>
    T Configuration::GetValue(_In_ const std::string& name, _In_ const T& defaultValue) const
    {
        static_assert(!std::is_pointer<T>::value, "GetValue only works with value types, not pointers.");
        static_assert(!std::is_reference<T>::value, "GetValue only works with value types, not references.");

        T value;
        if (!stde::from_string(GetStringValue(name), value))
            value = defaultValue;
        return value;
    }

    template <typename T>
    void Configuration::SetValue(_In_ const std::string& name, _In_ const T& value)
    {
        static_assert(!std::is_pointer<T>::value, "SetValue only works with value types, not pointers.");
        static_assert(!std::is_reference<T>::value, "SetValue only works with value types, not references.");

        SetStringValue(name, stde::to_string(value));
    }

    inline bool __parseFloats(_Inout_ std::istringstream& ss, _In_ size_t numFloats, _Inout_cap_(numFloats) float* pFloats)
    {
        std::string s;

        float* pNext = pFloats;
        float value;
        for (size_t i = 0; i < numFloats - 1; i++)
        {
            std::getline( ss, s, ',' );
            if (!stde::from_string(s, value))
                return false;

            *pNext = value;
            pNext++;
        }

        std::getline(ss, s, ';');
        if (!stde::from_string(s, value))
            return false;

        *pNext = value;
        return true;
    }

    inline void __storeFloats(_Inout_ std::ostringstream& ss, _In_ size_t numFloats, _In_count_(numFloats) const float* pFloats)
    {
        const float* pNext = pFloats;
        for (size_t i = 0; i < numFloats - 1; i++)
        {
            ss << *pNext << ',';
            pNext++;
        }

        ss << *pNext << ';';
    }

    // Specializations for DirectX && Lucid3D types
    template <>
    inline DirectX::XMFLOAT2 Configuration::GetValue(_In_ const std::string& name, _In_ const DirectX::XMFLOAT2& defaultValue) const
    {
        std::istringstream ss(GetStringValue(name));
        DirectX::XMFLOAT2 value;

        if (!__parseFloats(ss, 2, &value.x))
            return defaultValue;

        return value;
    }

    template <>
    inline DirectX::XMFLOAT3 Configuration::GetValue(_In_ const std::string& name, _In_ const DirectX::XMFLOAT3& defaultValue) const
    {
        std::istringstream ss(GetStringValue(name));
        DirectX::XMFLOAT3 value;

        if (!__parseFloats(ss, 3, &value.x))
            return defaultValue;

        return value;
    }

    template <>
    inline DirectX::XMFLOAT4 Configuration::GetValue(_In_ const std::string& name, _In_ const DirectX::XMFLOAT4& defaultValue) const
    {
        std::istringstream ss(GetStringValue(name));
        DirectX::XMFLOAT4 value;

        if (!__parseFloats(ss, 4, &value.x))
            return defaultValue;

        return value;
    }

    template <>
    inline void Configuration::SetValue(_In_ const std::string& name, _In_ const DirectX::XMFLOAT2& value)
    {
        std::ostringstream ss;
        __storeFloats(ss, 2, &value.x);
        SetStringValue(name, ss.str());
    }

    template <>
    inline void Configuration::SetValue(_In_ const std::string& name, _In_ const DirectX::XMFLOAT3& value)
    {
        std::ostringstream ss;
        __storeFloats(ss, 3, &value.x);
        SetStringValue(name, ss.str());
    }

    template <>
    inline void Configuration::SetValue(_In_ const std::string& name, _In_ const DirectX::XMFLOAT4& value)
    {
        std::ostringstream ss;
        __storeFloats(ss, 4, &value.x);
        SetStringValue(name, ss.str());
    }

    // Specializations for GDK::Transform
    template <>
    inline GDK::Transform Configuration::GetValue(_In_ const std::string& name, _In_ const GDK::Transform& defaultValue) const
    {
        std::istringstream ss(GetStringValue(name));
        GDK::Transform transform;

        if (!__parseFloats(ss, 3, &transform.Position.x))
            return defaultValue;

        if (!__parseFloats(ss, 4, &transform.Orientation.x))
            return defaultValue;

        if (!__parseFloats(ss, 3, &transform.Scale.x))
            return defaultValue;

        return transform;
    }

    template <>
    inline void Configuration::SetValue(_In_ const std::string& name, _In_ const GDK::Transform& value)
    {
        std::ostringstream ss;
        __storeFloats(ss, 3, &value.Position.x);
        __storeFloats(ss, 4, &value.Orientation.x);
        __storeFloats(ss, 3, &value.Scale.x);
        SetStringValue(name, ss.str());
    }

}

