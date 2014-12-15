#pragma once
#include <GDK.h>
#include <StringUtils.h>
#include "debug.h"

using namespace GDK;

class QuakeProperties : public GDK::RuntimeObject<QuakeProperties>
{
public:
    static std::shared_ptr<QuakeProperties> Create(_In_ const std::map<std::wstring, std::wstring>& properties)
    {
        return std::shared_ptr<QuakeProperties>(GDKNEW QuakeProperties(properties));
    }

    float GetDelay()
    {
        auto prop = _properties.find(L"delay");
        if (prop != _properties.end())
        {
            return StringUtils::Parse<float>(prop->second);
        }

        return 0.0f;
    }

    float GetWait()
    {
        auto prop = _properties.find(L"wait");
        if (prop != _properties.end())
        {
            return StringUtils::Parse<float>(prop->second);
        }

        return 0.0f;
    }

    std::wstring GetClassName()
    {
        return _properties[L"classname"];
    }

    std::wstring GetTarget()
    {
        // Target seems to be overloaded at times with killtarget
        auto prop = _properties.find(L"killtarget");
        if (prop != _properties.end())
        {
            return _properties[L"killtarget"];
        }
        return _properties[L"target"];
    }

    std::wstring GetTargetName()
    {
        return _properties[L"targetname"];
    }

    GDK::Vector3 GetOrigin()
    {
        auto prop = _properties.find(L"origin");
        if (prop != _properties.end())
        {
            return StringUtils::Parse<Vector3>(prop->second);
        }

        return GDK::Vector3::Zero();
    }

    std::wstring GetSoundName()
    {
        return L"sound/" + _properties[L"noise"];
    }

    std::wstring GetMessage()
    {
        auto prop = _properties.find(L"message");
        if (prop != _properties.end())
        {
            return _properties[L"message"];
        }
        else
        {
            return L"";
        }
    }

    float GetAngle()
    {
        auto prop = _properties.find(L"angle");
        if (prop != _properties.end())
        {
            return Math::ToRadians(StringUtils::Parse<float>(prop->second));
        }

        return 0.0f;
    }

    float GetSpeed()
    {
        auto prop = _properties.find(L"speed");
        if (prop != _properties.end())
        {
            return StringUtils::Parse<float>(prop->second);
        }

        return 0.0f;
    }

    int GetModel()
    {
        auto prop = _properties.find(L"model");
        if (prop != _properties.end())
        {
            std::wstring modelName = prop->second;
            std::wstring modelId = modelName.data() + 1;
            return StringUtils::Parse<int>(modelId);
        }

        return -1;
    }

private:
    QuakeProperties(_In_ const std::map<std::wstring, std::wstring>& properties)
    {
        _properties = properties;
    }

    std::map<std::wstring, std::wstring> _properties;
};