#pragma once

namespace VRES
{
    struct Point
    {
        int X;
        int Y;

        Point() : X(0), Y(0) {}
        Point(int x, int y) : X(x), Y(y) {}
    };

    struct Size
    {
        int Width;
        int Height;

        Size() : Width(0), Height(0) {}
        Size(int w, int h) : Width(w), Height(h) {}
    };

    struct BoundingBox
    {
        XMFLOAT3 Center;
        XMFLOAT3 Extents;

        BoundingBox() : Center(0, 0, 0), Extents(0, 0, 0) {};
        BoundingBox(const XMFLOAT3& center, const XMFLOAT3& extents) :
            Center(center), Extents(extents)
        {}

        XMFLOAT3 Min() const
        {
            return XMFLOAT3(Center.x - Extents.x,
                            Center.y - Extents.y,
                            Center.z - Extents.z);
        }

        XMFLOAT3 Max() const
        {
            return XMFLOAT3(Center.x + Extents.x,
                            Center.y + Extents.y,
                            Center.z + Extents.z);
        }
    };

    // Template parameter is just for ease of use. It shows up in error messages
    // when you try and copy a derived class, so you can easily determine which
    // class you are trying to copy incorrectly.
    template <class TDerived>
    class NonCopyable
    {
    public:
        NonCopyable() {}

    private:
        NonCopyable(const NonCopyable&);
        NonCopyable& operator= (const NonCopyable&);
    };
}
