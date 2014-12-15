#pragma once

#include <Platform.h>
#include <MathUtil.h>
#include <Vector3.h>

namespace GDK
{
    class GeometryContent;

    #pragma pack(push, 1)   // these structures can be used in file I/O, and so must be packed to 1 byte packing

    struct Float2
    {
        Float2() {}
        Float2(float x, float y) : x(x), y(y) {}

        float x;
        float y;
    };

    struct Float3
    {
        Float3() {}
        Float3(float x, float y, float z) : x(x), y(y), z(z) {}

        float x;
        float y;
        float z;
    };

    template <typename T>
    struct Rectangle
    {
        Rectangle() : 
            left(static_cast<T>(0)), 
            top(static_cast<T>(0)), 
            right(static_cast<T>(0)), 
            bottom(static_cast<T>(0))
        {}

        Rectangle(_In_ T left, _In_ T top, _In_ T width, _In_ T height) 
            : left(left), top(top), right(left + width), bottom(top + height) {}

        T Width() const { return right - left; }
        T Height() const { return bottom - top; }

        T left, top, right, bottom;
    };

    typedef Rectangle<float> RectangleF;

    #pragma pack(pop)
}
