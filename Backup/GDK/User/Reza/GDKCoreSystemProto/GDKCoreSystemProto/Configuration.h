#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_CONFIGURATION_H_
#define _GDK_CONFIGURATION_H_

namespace GDK
{
    struct StringID;

    class Configuration
    {
    public:
        // Load configuration from storage
        static HRESULT GDK_API          Load(_In_ const wchar_t* filename);

        // Save configuration to storage
        static HRESULT GDK_API          Save(_In_ const wchar_t* filename);

        // Clear all values
        static void GDK_API             Clear();

        // Getting values
        static bool GDK_API             GetBool(_In_ const StringID& key, _In_ bool defaultValue = false);
        static int64_t GDK_API          GetInt(_In_ const StringID& key, _In_ int64_t defaultValue = 0);
        static double GDK_API           GetReal(_In_ const StringID& key, _In_ double defaultValue = 0.0);
        static const wchar_t* GDK_API   GetString(_In_ const StringID& key, _In_ const wchar_t* defaultValue = L"");

        // Setting and creating new values
        static HRESULT GDK_API          SetValue(_In_ const StringID& key, _In_ bool value);
        static HRESULT GDK_API          SetValue(_In_ const StringID& key, _In_ int64_t value);
        static HRESULT GDK_API          SetValue(_In_ const StringID& key, _In_ double value);
        static HRESULT GDK_API          SetValue(_In_ const StringID& key, _In_ const wchar_t* value);

    private:
        // This is meant to be a static class, so don't let anyone create an instance
        Configuration();
        Configuration(const Configuration&);
        Configuration& operator= (const Configuration&);
    };
} // GDK

#endif // _GDK_CONFIGURATION_H_
