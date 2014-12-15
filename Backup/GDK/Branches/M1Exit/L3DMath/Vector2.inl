// Implementation of Vector2
#pragma once

#include <math.h>

namespace L3DMath
{
    inline Vector2::Vector2()
        : X(0.0f), Y(0.0f)
    {
    }

    inline Vector2::Vector2(float x, float y)
        : X(x), Y(y)
    {
    }

    inline Vector2::Vector2(const Vector2& v)
        : X(v.X), Y(v.Y)
    {
    }

#ifdef INTEROP
    // implict cast constructors
    inline Vector2::Vector2(const DirectX::XMFLOAT2& xmf)
        : X(xmf.x), Y(xmf.y)
    {
    }

    inline Vector2::Vector2(const DirectX::XMVECTOR& xmv)
    {
        DirectX::XMFLOAT2 f;
        DirectX::XMStoreFloat2(&f, xmv);
        *this = f;
    }

    // conversions
    inline Vector2::operator DirectX::XMFLOAT2() const
    {
        return DirectX::XMFLOAT2(X, Y);
    }

    inline Vector2::operator DirectX::XMVECTOR() const
    {
        DirectX::XMFLOAT2 f(X, Y);
        return DirectX::XMLoadFloat2(&f);
    }

    // assignment
    inline Vector2& Vector2::operator= (const DirectX::XMFLOAT2& xmf)
    {
        X = xmf.x;
        Y = xmf.y;
        return *this;
    }

    inline Vector2& Vector2::operator= (const DirectX::XMVECTOR& xmv)
    {
        DirectX::XMFLOAT2 f;
        DirectX::XMStoreFloat2(&f, xmv);
        *this = f;
        return *this;
    }
#endif

    inline Vector2& Vector2::operator= (const Vector2& v)
    {
        X = v.X;
        Y = v.Y;
        return *this;
    }

    // Element Access
    inline Vector2& Vector2::Set(float x, float y)
    {
        X = x;
        Y = y;
        return *this;
    }

    inline Vector2& Vector2::SetX(float x)
    {
        X = x;
        return *this;
    }

    inline Vector2& Vector2::SetY(float y)
    {
        Y = y;
        return *this;
    }

    inline float&   Vector2::operator[](unsigned int n)
    {
        return n == 0 ? X : Y;
    }
    
    // Operations
    inline float        Vector2::Dot(const Vector2& v1, const Vector2& v2)
    {
        return v1.X * v2.X + v1.Y * v2.Y;
    }

    inline Vector2      Vector2::Normalize(const Vector2& v)
    {
        float oneOverLength = 1.0f / v.Length();
        return v * oneOverLength;
    }

    inline float        Vector2::Length() const
    {
        return sqrt(LengthSquared());
    }

    inline float        Vector2::LengthSquared() const
    {
        return X * X + Y * Y;
    }

    inline float        Vector2::Dot(const Vector2& v)
    {
        return Dot(*this, v);
    }

    inline Vector2&     Vector2::Normalize()
    {
        float oneOverLength = 1.0f / Length();
        X *= oneOverLength;
        Y *= oneOverLength;
        return *this;
    }

    inline Vector2 Vector2::Zero()
    {
        return Vector2(0.0f, 0.0f);
    }

    inline Vector2 Vector2::One()
    {
        return Vector2(1.0f, 1.0f);
    }

    inline Vector2 Vector2::UnitX()
    {
        return Vector2(1.0f, 0.0f);
    }

    inline Vector2 Vector2::UnitY()
    {
        return Vector2(0.0f, 1.0f);
    }

    inline Vector2 Vector2::Up()
    {
        return Vector2(0.0f, 1.0f);
    }

    inline Vector2 Vector2::Down()
    {
        return Vector2(0.0f, -1.0f);
    }

    inline Vector2 Vector2::Left()
    {
        return Vector2(-1.0f, 0.0f);
    }

    inline Vector2 Vector2::Right()
    {
        return Vector2(1.0f, 0.0f);
    }

    // negate
    inline Vector2 operator-(const Vector2& v)
    {
        return v * -1.0f;
    }

    // Addition, Subtraction, Scalar multiplication
    inline Vector2 operator+(const Vector2& v1, const Vector2& v2)
    {
        return Vector2(v1.X + v2.X, v1.Y + v2.Y);
    }

    inline Vector2& operator+=(Vector2& v1, const Vector2& v2)
    {
        v1 = v1 + v2;
        return v1;
    }

    inline Vector2 operator-(const Vector2& v1, const Vector2& v2)
    {
        return Vector2(v1.X - v2.X, v1.Y - v2.Y);
    }

    inline Vector2& operator-=(Vector2& v1, const Vector2& v2)
    {
        v1 = v1 - v2;
        return v1;
    }

    inline Vector2 operator*(const Vector2& v, const float& s)
    {
        return Vector2(v.X * s, v.Y * s);
    }

    inline Vector2& operator*=(Vector2& v, const float& s)
    {
        v = v * s;
        return v;
    }

    inline Vector2 operator*(const float& s, const Vector2& v)
    {
        return v * s;
    }
}

