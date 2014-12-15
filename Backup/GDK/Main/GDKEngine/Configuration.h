#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_CONFIGURATION_H_
#define _GDK_CONFIGURATION_H_

namespace GDK
{
    class Configuration : public RefCounted<>
    {
    public:
        // Create a new empty configuration
        static GDK_METHOD Create(_Deref_out_ Configuration** configuration);

        // Load configuration from storage
        static GDK_METHOD Load(_In_ IStream* stream, _Deref_out_ Configuration** configuration);

        // Save configuration to storage
        GDK_METHOD Save(_In_ IStream* stream);

        // Clear all values
        void GDK_API Clear();

        // Getting values
        GDK_METHOD_(bool)           GetBool(_In_ const wchar_t* key, _In_ bool defaultValue = false) const;
        GDK_METHOD_(int32_t)        GetInt(_In_ const wchar_t* key, _In_ int32_t defaultValue = 0) const;
        GDK_METHOD_(float)          GetReal(_In_ const wchar_t* key, _In_ float defaultValue = 0.0f) const;
        GDK_METHOD_(const wchar_t*) GetString(_In_ const wchar_t* key, _In_ const wchar_t* defaultValue = L"") const;

        // Setting and creating new values
        GDK_METHOD SetValue(_In_ const wchar_t* key, _In_ bool value);
        GDK_METHOD SetValue(_In_ const wchar_t* key, _In_ int32_t value);
        GDK_METHOD SetValue(_In_ const wchar_t* key, _In_ float value);
        GDK_METHOD SetValue(_In_ const wchar_t* key, _In_ const wchar_t* value);

    private:
        Configuration();
        ~Configuration();

        class ConfigEntry;

        uint16_t _maxEntries;
        uint16_t _numEntries;
        ConfigEntry** _entries;
    };
} // GDK

#endif // _GDK_CONFIGURATION_H_
