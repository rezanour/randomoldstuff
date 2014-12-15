#include "StdAfx.h"
#include "StringID.h"

using GDK::StringID;

const StringID StringID::Invalid(static_cast<uint64_t>(-1));

StringID::StringID()
    : _value(Invalid._value)
{
}

StringID::StringID(_In_ size_t value)
    : _value(value)
{
}

StringID::StringID(_In_ const StringID& other)
    : _value(other._value)
{
}

StringID& StringID::operator= (_In_ const StringID& other)
{
    _value = other._value;
    return *this;
}

bool StringID::operator== (_In_ const StringID& other) const
{
    return _value == other._value;
}

bool StringID::operator!= (_In_ const StringID& other) const
{
    return _value != other._value;
}

bool StringID::operator< (_In_ const StringID& other) const
{
    return _value < other._value;
}

bool StringID::operator> (_In_ const StringID& other) const
{
    return _value > other._value;
}

StringID::operator bool() const
{
    return *this != Invalid;
}

namespace GDK
{
    namespace Details
    {
        // TODO:
        // NOTES: This is a really unoptimized, but simple string table.
        // We do not bother removing/deleting strings. They are meant to live
        // the life of the process, and then get freed when the process is torn down
        static size_t g_numStrings = 0;
        static size_t g_maxStrings = 1000;
        static size_t g_reallocSize = 1000;
        static const wchar_t** g_strings = nullptr;
    }
}

StringID GDK_API StringID::RegisterString(_In_ const wchar_t* value)
{
    using namespace Details;

    if (!g_strings)
    {
        // if the table doesn't exist yet, create it
        g_strings = new const wchar_t*[g_maxStrings];
        if (!g_strings)
            return StringID::Invalid;
    }
    else if (g_numStrings == g_maxStrings)
    {
        // if the current table is full, grow it
        size_t newSize = g_maxStrings + g_reallocSize;
        const wchar_t** newTable = new const wchar_t*[newSize];
        if (!newTable)
            return StringID::Invalid;

        memcpy_s(static_cast<void*>(newTable), newSize * sizeof(wchar_t*), g_strings, g_numStrings * sizeof(wchar_t*));
        delete[] g_strings;
        g_strings = newTable;
        g_maxStrings = newSize;
    }

    // look for existing matching string
    for (size_t i = 0; i < g_numStrings; ++i)
    {
        if (wcscmp(value, g_strings[i]) == 0)
        {
            return StringID(i);
        }
    }

    g_strings[g_numStrings] = _wcsdup(value);
    return StringID(g_numStrings++);
}

StringID GDK_API StringID::GetStringID(_In_ const wchar_t* value)
{
    using namespace Details;

    if (!g_strings)
        return StringID::Invalid;

    for (size_t i = 0; i < g_numStrings; ++i)
    {
        if (wcscmp(value, g_strings[i]) == 0)
        {
            return StringID(i);
        }
    }

    return StringID::Invalid;
}

const wchar_t* GDK_API StringID::GetString(_In_ const StringID& stringID)
{
    using namespace Details;

    if (!g_strings)
        return nullptr;

    if (stringID._value >= g_numStrings)
        return nullptr;

    return g_strings[stringID._value];
}
