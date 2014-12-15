#pragma once

// Implementation of Vector3

#include "Matrix.h"

namespace GDK
{
    inline Vector3::Vector3()
        : x(0.0f), y(0.0f), z(0.0f)
    {
    }

    inline Vector3::Vector3(float x, float y, float z)
        : x(x), y(y), z(z)
    {
    }

    inline Vector3::Vector3(const Vector3& v)
        : x(v.x), y(v.y), z(v.z)
    {
    }

#ifdef DIRECTXMATH_INTEROP
    // implict cast constructors
    inline Vector3::Vector3(const DirectX::XMFLOAT3& xmf)
        : x(xmf.x), y(xmf.y), z(xmf.z)
    {
    }

    inline Vector3::Vector3(const DirectX::XMVECTOR& xmv)
    {
        DirectX::XMFLOAT3 f;
        DirectX::XMStoreFloat3(&f, xmv);
        *this = f;
    }

    // conversions
    inline Vector3::operator DirectX::XMFLOAT3() const
    {
        return DirectX::XMFLOAT3(x, y, z);
    }

    inline Vector3::operator DirectX::XMVECTOR() const
    {
        DirectX::XMFLOAT3 f(x, y, z);
        return DirectX::XMLoadFloat3(&f);
    }

    // assignment
    inline Vector3& Vector3::operator= (const DirectX::XMFLOAT3& xmf)
    {
        x = xmf.x;
        y = xmf.y;
        z = xmf.z;
        return *this;
    }

    inline Vector3& Vector3::operator= (const DirectX::XMVECTOR& xmv)
    {
        DirectX::XMFLOAT3 f;
        DirectX::XMStoreFloat3(&f, xmv);
        *this = f;
        return *this;
    }
#endif

    inline Vector3& Vector3::operator= (const Vector3& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    // Element Access
    inline Vector3& Vector3::Set(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        return *this;
    }

    inline Vector3& Vector3::SetX(float x)
    {
        this->x = x;
        return *this;
    }

    inline Vector3& Vector3::SetY(float y)
    {
        this->y = y;
        return *this;
    }

    inline Vector3& Vector3::SetZ(float z)
    {
        this->z = z;
        return *this;
    }

    inline float&   Vector3::operator[](unsigned int n)
    {
        return n == 0 ? x : (n == 1 ? y : z);
    }
    
    // Operations
    inline float        Vector3::Dot(const Vector3& v1, const Vector3& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    inline Vector3      Vector3::Cross(const Vector3& v1, const Vector3& v2)
    {
        return Vector3(v1.y * v2.z - v2.y * v1.z, 
                       v1.z * v2.x - v2.z * v1.x,
                       v1.x * v2.y - v2.x * v1.y);
    }

    inline Vector3      Vector3::Normalize(const Vector3& v)
    {
        float oneOverLength = 1.0f / v.Length();
        return v * oneOverLength;
    }

    inline Vector3      Vector3::Transform(const Vector3& v, const Matrix& m)
    {
        return v * m;
    }

    inline float        Vector3::Length() const
    {
        return sqrt(LengthSquared());
    }

    inline float        Vector3::LengthSquared() const
    {
        return x * x + y * y + z * z;
    }

    inline float        Vector3::Dot(const Vector3& v)
    {
        return Dot(*this, v);
    }

    inline Vector3      Vector3::Cross(const Vector3& v)
    {
        return Cross(*this, v);
    }

    inline Vector3&     Vector3::Normalize()
    {
        float oneOverLength = 1.0f / Length();
        x *= oneOverLength;
        y *= oneOverLength;
        z *= oneOverLength;
        return *this;
    }

    inline Vector3&     Vector3::Transform(const Matrix& m)
    {
        *this = *this * m;
        return *this;
    }

    inline Vector3 Vector3::Zero()
    {
        return Vector3(0.0f, 0.0f, 0.0f);
    }

    inline Vector3 Vector3::One()
    {
        return Vector3(1.0f, 1.0f, 1.0f);
    }

    inline Vector3 Vector3::UnitX()
    {
        return Vector3(1.0f, 0.0f, 0.0f);
    }

    inline Vector3 Vector3::UnitY()
    {
        return Vector3(0.0f, 1.0f, 0.0f);
    }

    inline Vector3 Vector3::UnitZ()
    {
        return Vector3(0.0f, 0.0f, 1.0f);
    }

    inline Vector3 Vector3::Up()
    {
        return Vector3(0.0f, 1.0f, 0.0f);
    }

    inline Vector3 Vector3::Down()
    {
        return Vector3(0.0f, -1.0f, 0.0f);
    }

    inline Vector3 Vector3::Left()
    {
        return Vector3(-1.0f, 0.0f, 0.0f);
    }

    inline Vector3 Vector3::Right()
    {
        return Vector3(1.0f, 0.0f, 0.0f);
    }

    inline Vector3 Vector3::Forward()
    {
        return Vector3(0.0f, 0.0f, 1.0f);
    }

    inline Vector3 Vector3::Backward()
    {
        return Vector3(0.0f, 0.0f, -1.0f);
    }

    // negate
    inline Vector3 operator-(const Vector3& v)
    {
        return v * -1.0f;
    }

    // Addition, Subtraction, Scalar multiplication
    inline Vector3 operator+(const Vector3& v1, const Vector3& v2)
    {
        return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    }

    inline Vector3& operator+=(Vector3& v1, const Vector3& v2)
    {
        v1 = v1 + v2;
        return v1;
    }

    inline Vector3 operator-(const Vector3& v1, const Vector3& v2)
    {
        return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }

    inline Vector3& operator-=(Vector3& v1, const Vector3& v2)
    {
        v1 = v1 - v2;
        return v1;
    }

    inline Vector3 operator*(const Vector3& v, const float& s)
    {
        return Vector3(v.x * s, v.y * s, v.z * s);
    }

    inline Vector3& operator*=(Vector3& v, const float& s)
    {
        v = v * s;
        return v;
    }

    inline Vector3 operator*(const float& s, const Vector3& v)
    {
        return v * s;
    }
}
