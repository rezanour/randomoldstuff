// conversion.h
// Convenience data conversion functions, such as wide string to string and vice versa

#pragma once

#include <sstream>
#include <algorithm>

namespace stde
{
    // converts most types to wstring
    template <typename T>
    inline std::wstring __fastcall to_wstring(_In_ const T& value)
    {
        std::wostringstream ss;
        ss << value;
        return ss.str();
    }

    // specialization for string
    template <>
    inline std::wstring __fastcall to_wstring(_In_ const std::string& str)
    {
        std::wstring ws(str.size(), L' ');
        std::copy(str.begin(), str.end(), ws.begin());
        return ws;
    }

    // converts most types to string
    template <typename T>
    inline std::string __fastcall to_string(_In_ const T& value)
    {
        std::ostringstream ss;
        ss << value;
        return ss.str();
    }

    // specialization for wstring
    template <>
    inline std::string __fastcall to_string(_In_ const std::wstring& ws)
    {
        std::string str(ws.size(), ' ');
        // we use transform here so we can explicitly cast from wchar_t to char to shut up the compiler warning about possible loss of data :)
        std::transform(ws.begin(), ws.end(), str.begin(), [](wchar_t c) { return (char)c; });
        return str;
    }

    // converts wstring to most types
    template <typename T>
    inline bool __fastcall from_wstring(_In_ const std::wstring& ws, _Inout_ T& result)
    {
        std::wistringstream ss(ws);
        ss >> result;
        return !ss.fail();
    }

    // converts sstring to most types
    template <typename T>
    inline bool __fastcall from_string(_In_ const std::string& str, _Inout_ T& result)
    {
        std::istringstream ss(str);
        ss >> result;
        return !ss.fail();
    }
}

