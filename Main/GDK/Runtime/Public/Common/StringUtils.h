#pragma once

#include <Platform.h>
#include <GDKMath.h>

namespace GDK
{
    namespace StringUtils
    {
        template <typename T>
        inline std::wstring ToString(_In_ const T& input)
        {
            std::wostringstream stream;
            stream << input;
            return stream.str();
        }

        template <typename T>
        inline T Parse(_In_ const std::wstring& input)
        {
            std::wistringstream stream(input);
            T value;
            stream >> value;
            return value;
        }

        template <>
        inline std::wstring ToString(_In_ const Vector3& input)
        {
            std::wostringstream stream;
            stream << input.x << L" " << input.y << L" " << input.z;
            return stream.str();
        }

        template <>
        inline Vector3 Parse(_In_ const std::wstring& input)
        {
            std::wistringstream stream(input);
            float x, y, z;
            stream >> x >> y >> z;
            return Vector3(x, y, z);
        }

    }
}
