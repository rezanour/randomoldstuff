#include "StdAfx.h"
#include "Configuration.h"
#include "Scripting.h"

using GDK::Configuration;
using GDK::StringID;
using GDK::Var;
using GDK::VarType;

#include <map>

namespace GDK
{
    namespace Details
    {
        typedef std::map<StringID, Var> ValueMap;
        static ValueMap g_values;

        static bool GDK_API ParseKeyValue(_Inout_ wchar_t*& line, _Out_ StringID* key, _Out_ Var* value);
    } // Details
} // GDK

HRESULT GDK_API Configuration::Load(_In_ const wchar_t* filename)
{
    using namespace GDK::Details;

    FATALP(filename, E_INVALIDARG);

    // Open the file
    HANDLE file = CreateFile(filename, GENERIC_READ, 0, nullptr, OPEN_ALWAYS, 0, nullptr);
    FATALB(file != INVALID_HANDLE_VALUE, HRESULT_FROM_WIN32(GetLastError()));

    // Get the total size of the file
    DWORD sizeHigh;
    DWORD sizeLow = GetFileSize(file, &sizeHigh);
    uint64_t totalSize = (static_cast<uint64_t>(sizeHigh) << 32) | sizeLow;
    CHECKB(totalSize > 0, E_UNEXPECTED);

    wchar_t* fileContents = new wchar_t[totalSize];
    FATALP(fileContents, E_OUTOFMEMORY);

    uint64_t totalBytesRead = 0;
    while (totalBytesRead < totalSize)
    {
        // compute the maximum number of bytes we can read (which still fits in a DWORD)
        DWORD bytesToRead = (totalSize - totalBytesRead) % static_cast<DWORD>(-1);
        DWORD bytesRead = 0;
        FATALB(ReadFile(file, static_cast<void*>(fileContents), bytesToRead, &bytesRead, nullptr), HRESULT_FROM_WIN32(GetLastError()));
        totalBytesRead += bytesRead;
    }

    // parse out the contents into our map
    wchar_t* cursor = fileContents;
    wchar_t* eof = fileContents + (totalSize / sizeof(wchar_t));
    wchar_t* line = nullptr;

    // make eof null terminator in case it's not
    *eof = 0;

    TrimUTFBOM(cursor);

    while (cursor < eof)
    {
        TrimCRLF(cursor);
        TrimWhitespaces(cursor);

        if (cursor == eof)
            break;

        // read a line in
        line = cursor;
        while (cursor < eof)
        {
            if (IsCRLFChar(*cursor))
            {
                break;
            }
            ++cursor;
        }

        // skip empty lines
        if (line == cursor)
            continue;

        // skip comment lines (starting with ';', '#', or "//")
        if (*line == L';' || *line == L'#' || StartsWith(line, L"//"))
            continue;

        // set end of line as newline
        *(cursor++) = 0;

        StringID key;
        Var value;
        FATALB(ParseKeyValue(line, &key, &value), E_UNEXPECTED);

        Details::g_values[key] = value;
    }

    delete[] fileContents;
    CloseHandle(file);

    return S_OK;
}

bool GDK_API Configuration::GetBool(_In_ const StringID& key, _In_ bool defaultValue)
{
    Details::ValueMap::iterator it = Details::g_values.find(key);
    return (it != Details::g_values.end() && it->second.IsBool()) ? it->second.ToBool() : defaultValue;
}

int64_t GDK_API Configuration::GetInt(_In_ const StringID& key, _In_ int64_t defaultValue)
{
    Details::ValueMap::iterator it = Details::g_values.find(key);
    return (it != Details::g_values.end() && it->second.IsInt()) ? it->second.ToInt() : defaultValue;
}

double GDK_API Configuration::GetReal(_In_ const StringID& key, _In_ double defaultValue)
{
    Details::ValueMap::iterator it = Details::g_values.find(key);
    return (it != Details::g_values.end() && it->second.IsReal()) ? it->second.ToReal() : defaultValue;
}

const wchar_t* GDK_API Configuration::GetString(_In_ const StringID& key, _In_ const wchar_t* defaultValue)
{
    Details::ValueMap::iterator it = Details::g_values.find(key);
    if (it != Details::g_values.end())
    {
        const wchar_t* value = StringID::GetString(it->second.ToString());
        return value ? value : defaultValue;
    }
    else
    {
        return defaultValue;
    }

}

bool GDK_API GDK::Details::ParseKeyValue(_Inout_ wchar_t*& line, _Out_ StringID* key, _Out_ Var* value)
{
    // ensure line is valid
    if (!line || *line == 0 || !key || !value)
        return false;

    TrimTrailingWhitespaces(line);

    // find equal sign, or error out if it doesn't exist
    wchar_t* equals = line;
    while (*equals != 0 && *equals != L'=')
    {
        ++equals;
    }

    if (*equals == 0)
        return false;

    // determine if we have more than one whitespace delimited token in the key
    wchar_t* p = line;
    wchar_t* firstWS = nullptr;
    while (p < equals)
    {
        if (*p == L' ' || *p == L'\t')
        {
            if (!firstWS)
            {
                firstWS = p;
            }
        }
        else if (firstWS)
        {
            break;
        }
        ++p;
    }

    if (p < equals)
    {
        size_t len = static_cast<size_t>(firstWS - line);
        // might have type info, see if token is a type
        if (len >= 3 && _wcsnicmp(line, L"int", 3) == 0)
        {
            value->SetType(VarType_Int);
            line = firstWS;
        }
        else if (len >= 4 && _wcsnicmp(line, L"real", 4) == 0)
        {
            value->SetType(VarType_Real);
            line = firstWS;
        }
        else if (len >= 4 && _wcsnicmp(line, L"bool", 4) == 0)
        {
            value->SetType(VarType_Bool);
            line = firstWS;
        }
        else
        {
            if (len >= 6 && _wcsnicmp(line, L"string", 6) == 0)
            {
                line = firstWS;
            }

            value->SetType(VarType_StringID);
        }
    }

    TrimWhitespaces(line);

    *equals = 0;
    TrimTrailingWhitespaces(line);

    *key = StringID::RegisterString(line);
    line = equals + 1;
    TrimWhitespaces(line);

    if (value->IsBool())
    {
        value->SetBool((*line == L't' || *line == L'T'));
    }
    else if (value->IsInt())
    {
        value->SetInt(_wtoi(line));
    }
    else if (value->IsReal())
    {
        value->SetReal(_wtof(line));
    }
    else
    {
        value->SetString(StringID::RegisterString(line));
    }

    return true;
}
