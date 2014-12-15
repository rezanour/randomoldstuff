#pragma once

#include <Platform.h>
#include <GDKMath.h>

namespace GDK
{
    struct VisualInfo
    {
        VisualInfo()
        {}

        VisualInfo(_In_ const Matrix& transform, _In_ const std::wstring& geometry, _In_ const std::wstring& texture) :
            transform(transform), geometry(geometry), texture(texture)
        {}

        Matrix transform;
        std::wstring geometry;
        std::wstring texture;
    };
}
