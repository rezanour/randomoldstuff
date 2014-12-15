#pragma once

// Implementation of Vector2

namespace GDK
{
    inline Vector2::Vector2()
        : x(0.0f), y(0.0f)
    {
    }

    inline Vector2::Vector2(float x, float y)
        : x(x), y(y)
    {
    }

    inline Vector2::Vector2(const Vector2& v)
        : x(v.x), y(v.y)
    {
    }

#ifdef DIRECTXMATH_INTEROP
    // implict cast constructors
    inline Vector2::Vector2(const DirectX::XMFLOAT2& xmf)
        : x(xmf.x), y(xmf.y)
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
        return DirectX::XMFLOAT2(x, y);
    }

    inline Vector2::operator DirectX::XMVECTOR() const
    {
        DirectX::XMFLOAT2 f(x, y);
        return DirectX::XMLoadFloat2(&f);
    }

    // assignment
    inline Vector2& Vector2::operator= (const DirectX::XMFLOAT2& xmf)
    {
        x = xmf.x;
        y = xmf.y;
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
        x = v.x;
        y = v.y;
        return *this;
    }

    // Element Access
    inline Vector2& Vector2::Set(float x, float y)
    {
        this->x = x;
        this->y = y;
        return *this;
    }

    inline Vector2& Vector2::SetX(float x)
    {
        this->x = x;
        return *this;
    }

    inline Vector2& Vector2::SetY(float y)
    {
        this->y = y;
        return *this;
    }

    inline float&   Vector2::operator[](unsigned int n)
    {
        return n == 0 ? x : y;
    }
    
    // Operations
    inline float        Vector2::Dot(const Vector2& v1, const Vector2& v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
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
        return x * x + y * y;
    }

    inline float        Vector2::Dot(const Vector2& v)
    {
        return Dot(*this, v);
    }

    inline Vector2&     Vector2::Normalize()
    {
        float oneOverLength = 1.0f / Length();
        x *= oneOverLength;
        y *= oneOverLength;
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
        return Vector2(v1.x + v2.x, v1.y + v2.y);
    }

    inline Vector2& operator+=(Vector2& v1, const Vector2& v2)
    {
        v1 = v1 + v2;
        return v1;
    }

    inline Vector2 operator-(const Vector2& v1, const Vector2& v2)
    {
        return Vector2(v1.x - v2.x, v1.y - v2.y);
    }

    inline Vector2& operator-=(Vector2& v1, const Vector2& v2)
    {
        v1 = v1 - v2;
        return v1;
    }

    inline Vector2 operator*(const Vector2& v, const float& s)
    {
        return Vector2(v.x * s, v.y * s);
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
