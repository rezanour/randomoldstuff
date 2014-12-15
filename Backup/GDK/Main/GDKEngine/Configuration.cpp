#include "StdAfx.h"

using GDK::Configuration;

namespace GDK
{
    namespace Details
    {
        static enum ConfigEntryType
        {
            CET_Bool,
            CET_Int,
            CET_Real,
            CET_String
        };
    }

    class Configuration::ConfigEntry
    {
    public:
        ConfigEntry(_In_ const wchar_t* key, _In_ bool value) : _key(_wcsdup(key)) { Set(value); }
        ConfigEntry(_In_ const wchar_t* key, _In_ int32_t value) : _key(_wcsdup(key)) { Set(value); }
        ConfigEntry(_In_ const wchar_t* key, _In_ float value) : _key(_wcsdup(key)) { Set(value); }
        ConfigEntry(_In_ const wchar_t* key, _In_ const wchar_t* value) : _key(_wcsdup(key)) { Set(value); }

        explicit ConfigEntry(_In_ const ConfigEntry& other)
            : _value(0)
        {
            *this = other;
        }

        ConfigEntry(_In_ ConfigEntry&& moveOther)
            : _key(moveOther._key), _type(moveOther._type), _value(moveOther._value)
        {
            // invalidate the data on the other so that it's destructor won't clean anything up
            moveOther._key = nullptr;
            moveOther._value = 0;
        }

        ConfigEntry& operator= (_In_ const ConfigEntry& other)
        {
            if (this == &other)
                return *this;

            Clear();

            _key = _wcsdup(other._key);
            _type = other._type;

            if (_type == Details::CET_String)
            {
                Set(other.GetString());
            }
            else
            {
                _value = other._value;
            }

            return *this;
        }

        ConfigEntry& operator= (_In_ ConfigEntry&& other)
        {
            Clear();
            _key = _wcsdup(other._key);
            other._key = nullptr;
            _type = other._type;
            _value = other._value;
            other._value = 0;
            return *this;
        }

        ~ConfigEntry()
        {
            Clear();
            if (_key)
            {
                free(_key);
            }
        }

        bool IsMatch(_In_ const wchar_t* key) const         { return _wcsicmp(key, _key) == 0; }
        bool IsTypeMatch(_In_ Details::ConfigEntryType type) const   { return _type == type; }

        bool GetBool() const                { return *(bool*)&_value; }
        int32_t GetInt() const              { return *(int32_t*)&_value; }
        float GetReal() const               { return *(float*)&_value; }
        const wchar_t* GetString() const    { return (wchar_t*)_value; }

        void Set(_In_ bool value)           { Clear(); _type = Details::CET_Bool; _value = *(uint64_t*)&value; }
        void Set(_In_ int32_t value)        { Clear(); _type = Details::CET_Int;  _value = *(uint64_t*)&value; }
        void Set(_In_ float value)          { Clear(); _type = Details::CET_Real; _value = *(uint64_t*)&value; }
        void Set(_In_ const wchar_t* value) { Clear(); _type = Details::CET_String; _value = (uint64_t)_wcsdup(value); }

    private:
        void Clear()
        {
            if (_type == Details::CET_String && _value != 0)
            {
                free((wchar_t*)_value);
            }
        }

        wchar_t* _key;
        Details::ConfigEntryType _type;
        uint64_t _value;
    };
}

Configuration::Configuration()
    : _maxEntries(0), _numEntries(0), _entries(nullptr)
{
}

Configuration::~Configuration()
{
    Clear();
    SafeDeleteArray(_entries);
    _maxEntries = 0;
}

GDK_METHOD Configuration::Create(_Deref_out_ Configuration** configuration)
{
    assert(configuration);

    *configuration = new Configuration;
    (*configuration)->_maxEntries = 500;
    (*configuration)->_entries = new ConfigEntry*[(*configuration)->_maxEntries];
    for (uint16_t i = 0; i < (*configuration)->_maxEntries; ++i)
    {
        (*configuration)->_entries[i] = nullptr;
    }

    return S_OK;
}

GDK_METHOD Configuration::Load(_In_ IStream* stream, _Deref_out_ Configuration** configuration)
{
    assert(stream);
    UNREFERENCED_PARAMETER(stream);
    CHK(Create(configuration));
    return S_OK;
}

GDK_METHOD Configuration::Save(_In_ IStream* stream)
{
    UNREFERENCED_PARAMETER(stream);
    return E_NOTIMPL;
}

GDK_METHOD_(void) Configuration::Clear()
{
    for (uint16_t i = 0; i < _numEntries; ++i)
    {
        SafeDelete(_entries[i]);
    }
    _numEntries = 0;
}

GDK_METHOD_(bool) Configuration::GetBool(_In_ const wchar_t* key, _In_ bool defaultValue) const
{
    for (uint16_t i = 0; i < _numEntries; ++i)
    {
        if (_entries[i] && _entries[i]->IsTypeMatch(Details::CET_Bool) && _entries[i]->IsMatch(key))
        {
            return _entries[i]->GetBool();
        }
    }

    return defaultValue;
}

GDK_METHOD_(int32_t) Configuration::GetInt(_In_ const wchar_t* key, _In_ int32_t defaultValue) const
{
    for (uint16_t i = 0; i < _numEntries; ++i)
    {
        if (_entries[i] && _entries[i]->IsTypeMatch(Details::CET_Int) && _entries[i]->IsMatch(key))
        {
            return _entries[i]->GetInt();
        }
    }

    return defaultValue;
}

GDK_METHOD_(float) Configuration::GetReal(_In_ const wchar_t* key, _In_ float defaultValue) const
{
    for (uint16_t i = 0; i < _numEntries; ++i)
    {
        if (_entries[i] && _entries[i]->IsTypeMatch(Details::CET_Real) && _entries[i]->IsMatch(key))
        {
            return _entries[i]->GetReal();
        }
    }

    return defaultValue;
}

GDK_METHOD_(const wchar_t*) Configuration::GetString(_In_ const wchar_t* key, _In_ const wchar_t* defaultValue) const
{
    for (uint16_t i = 0; i < _numEntries; ++i)
    {
        if (_entries[i] && _entries[i]->IsTypeMatch(Details::CET_String) && _entries[i]->IsMatch(key))
        {
            return _entries[i]->GetString();
        }
    }

    return defaultValue;
}

GDK_METHOD Configuration::SetValue(_In_ const wchar_t* key, _In_ bool value)
{
    for (uint16_t i = 0; i < _numEntries; ++i)
    {
        if (_entries[i] && _entries[i]->IsMatch(key))
        {
            if (_entries[i]->IsTypeMatch(Details::CET_Bool))
            {
                _entries[i]->Set(value);
            }
            else
            {
                return HRESULT_FROM_WIN32(ERROR_INVALID_DATATYPE);
            }
        }
    }

    // no match found, add a new one
    _entries[_numEntries++] = new ConfigEntry(key, value);
    return S_OK;
}

GDK_METHOD Configuration::SetValue(_In_ const wchar_t* key, _In_ int32_t value)
{
    for (uint16_t i = 0; i < _numEntries; ++i)
    {
        if (_entries[i] && _entries[i]->IsMatch(key))
        {
            if (_entries[i]->IsTypeMatch(Details::CET_Int))
            {
                _entries[i]->Set(value);
            }
            else
            {
                return HRESULT_FROM_WIN32(ERROR_INVALID_DATATYPE);
            }
        }
    }

    // no match found, add a new one
    _entries[_numEntries++] = new ConfigEntry(key, value);
    return S_OK;
}

GDK_METHOD Configuration::SetValue(_In_ const wchar_t* key, _In_ float value)
{
    for (uint16_t i = 0; i < _numEntries; ++i)
    {
        if (_entries[i] && _entries[i]->IsMatch(key))
        {
            if (_entries[i]->IsTypeMatch(Details::CET_Real))
            {
                _entries[i]->Set(value);
            }
            else
            {
                return HRESULT_FROM_WIN32(ERROR_INVALID_DATATYPE);
            }
        }
    }

    // no match found, add a new one
    _entries[_numEntries++] = new ConfigEntry(key, value);
    return S_OK;
}

GDK_METHOD Configuration::SetValue(_In_ const wchar_t* key, _In_ const wchar_t* value)
{
    for (uint16_t i = 0; i < _numEntries; ++i)
    {
        if (_entries[i] && _entries[i]->IsMatch(key))
        {
            if (_entries[i]->IsTypeMatch(Details::CET_String))
            {
                _entries[i]->Set(value);
            }
            else
            {
                return HRESULT_FROM_WIN32(ERROR_INVALID_DATATYPE);
            }
        }
    }

    // no match found, add a new one
    _entries[_numEntries++] = new ConfigEntry(key, value);
    return S_OK;
}
