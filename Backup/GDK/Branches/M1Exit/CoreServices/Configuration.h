// Configuration file reading and writing
#pragma once

#include <Windows.h>
#include <memory>
#include <string>
#include <map>

#include <stde\non_copyable.h>
#include <stde\com_ptr.h>

namespace CoreServices
{
    class Configuration;
    typedef std::shared_ptr<Configuration> ConfigurationPtr;

    class Configuration : stde::non_copyable
    {
    public:
        virtual ~Configuration();

        // Creation

        static HRESULT Create(_Out_ ConfigurationPtr* pspConfiguration);
        static HRESULT Load(_In_ const std::wstring& filePath, _Out_ ConfigurationPtr* pspConfiguration);
        static HRESULT Load(_In_ stde::com_ptr<IStream> spStream, _Out_ ConfigurationPtr* pspConfiguration);

        // Saving

        HRESULT Save(_In_ const std::wstring& filePath);
        HRESULT Save(_In_ stde::com_ptr<IStream> spStream);

        // Setting Values

        size_t GetCount() const;

        std::string GetValueNameAt(_In_ size_t index) const;

        std::string GetStringValue(_In_ const std::string& name, _In_ const char* defaultValue = "") const;
        void SetStringValue(_In_ const std::string& name, _In_ const std::string& value);

        template <typename T>
        T GetValue(_In_ const std::string& name, _In_ const T& defaultValue) const;

        template <typename T>
        void SetValue(_In_ const std::string& name, _In_ const T& value);

    private:
        Configuration();

        typedef std::map<std::string, std::string> SettingMap;
        SettingMap _settings;
    };
}

#include "Configuration.inl"

