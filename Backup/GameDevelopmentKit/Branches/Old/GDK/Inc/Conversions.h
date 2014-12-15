#ifndef _CONVERSIONS_H_
#define _CONVERSIONS_H_

#include <sstream>
#include <algorithm>

namespace GDK
{
    // converts most types to wstring
    template <typename T>
    inline std::wstring GDKAPI ConvertToString(_In_ const T& value)
    {
        std::wostringstream ss;
        ss << value;
        return ss.str();
    }

    // specialization for string
    template <>
    inline std::wstring GDKAPI ConvertToString(_In_ const std::string& str)
    {
        std::wstring ws(str.size(), L' ');
        std::copy(str.begin(), str.end(), ws.begin());
        return ws;
    }

    // converts wstring to most types
    template <typename T>
    inline bool GDKAPI ConvertFromString(_In_ const std::wstring& ws, _Inout_ T& result)
    {
        std::wistringstream ss(ws);
        ss >> result;
        return !ss.fail();
    }

    // converts sstring to most types
    template <typename T>
    inline bool GDKAPI ConvertFromString(_In_ const std::string& str, _Inout_ T& result)
    {
        std::istringstream ss(str);
        ss >> result;
        return !ss.fail();
    }
} // GDK

#endif // _CONVERSIONS_H_