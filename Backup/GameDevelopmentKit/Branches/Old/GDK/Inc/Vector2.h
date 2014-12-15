#pragma once

// 2 dimensional vector

namespace GDK
{
    class Vector2
    {
    public:
        // Construction
        Vector2();
        Vector2(float x, float y);
        Vector2(const Vector2& v);

#ifdef DIRECTXMATH_INTEROP
        // implict cast constructors
        Vector2(const DirectX::XMFLOAT2& xmf);
        Vector2(const DirectX::XMVECTOR& xmv);

        // conversions
        operator DirectX::XMFLOAT2 () const;
        operator DirectX::XMVECTOR () const;

        // assignment
        Vector2& operator= (const DirectX::XMFLOAT2& xmf);
        Vector2& operator= (const DirectX::XMVECTOR& xmv);
#endif

        // Assignment
        Vector2& operator= (const Vector2& v);

        // Element Access
        Vector2&    Set(float x, float y);
        Vector2&    SetX(float x);
        Vector2&    SetY(float y);
        float& operator[](unsigned int n);

        // Operations
        static float    Dot(const Vector2& v1, const Vector2& v2);
        static Vector2  Normalize(const Vector2& v);

        float           Length() const;
        float           LengthSquared() const;
        float           Dot(const Vector2& v);
        Vector2&        Normalize();

        // Static Vectors
        static Vector2  Zero();
        static Vector2  One();
        static Vector2  UnitX();
        static Vector2  UnitY();
        static Vector2  Up();
        static Vector2  Down();
        static Vector2  Left();
        static Vector2  Right();

        float x, y;
    };

    // negate
    Vector2 operator-(const Vector2& v);

    // Addition, Subtraction, Scalar multiplication
    Vector2 operator+(const Vector2& v1, const Vector2& v2);
    Vector2& operator+=(Vector2& v1, const Vector2& v2);
    Vector2 operator-(const Vector2& v1, const Vector2& v2);
    Vector2& operator-=(Vector2& v1, const Vector2& v2);
    Vector2 operator*(const Vector2& v, const float& s);
    Vector2& operator*=(Vector2& v, const float& s);
    Vector2 operator*(const float& s, const Vector2& v);
}

#include <Vector2.inl>