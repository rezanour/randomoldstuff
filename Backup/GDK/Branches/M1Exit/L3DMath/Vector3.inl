// Implementation of Vector3
#pragma once

#include "Matrix.h"
#include <math.h>

namespace L3DMath
{
    inline Vector3::Vector3()
        : X(0.0f), Y(0.0f), Z(0.0f)
    {
    }

    inline Vector3::Vector3(float x, float y, float z)
        : X(x), Y(y), Z(z)
    {
    }

    inline Vector3::Vector3(const Vector3& v)
        : X(v.X), Y(v.Y), Z(v.Z)
    {
    }

#ifdef INTEROP
    // implict cast constructors
    inline Vector3::Vector3(const DirectX::XMFLOAT3& xmf)
        : X(xmf.x), Y(xmf.y), Z(xmf.z)
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
        return DirectX::XMFLOAT3(X, Y, Z);
    }

    inline Vector3::operator DirectX::XMVECTOR() const
    {
        DirectX::XMFLOAT3 f(X, Y, Z);
        return DirectX::XMLoadFloat3(&f);
    }

    // assignment
    inline Vector3& Vector3::operator= (const DirectX::XMFLOAT3& xmf)
    {
        X = xmf.x;
        Y = xmf.y;
        Z = xmf.z;
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
        X = v.X;
        Y = v.Y;
        Z = v.Z;
        return *this;
    }

    // Element Access
    inline Vector3& Vector3::Set(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = z;
        return *this;
    }

    inline Vector3& Vector3::SetX(float x)
    {
        X = x;
        return *this;
    }

    inline Vector3& Vector3::SetY(float y)
    {
        Y = y;
        return *this;
    }

    inline Vector3& Vector3::SetZ(float z)
    {
        Z = z;
        return *this;
    }

    inline float&   Vector3::operator[](unsigned int n)
    {
        return n == 0 ? X : (n == 1 ? Y : Z);
    }
    
    // Operations
    inline float        Vector3::Dot(const Vector3& v1, const Vector3& v2)
    {
        return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
    }

    inline Vector3      Vector3::Cross(const Vector3& v1, const Vector3& v2)
    {
        return Vector3(v1.Y * v2.Z - v2.Y * v1.Z, 
                       v1.Z * v2.X - v2.Z * v1.X,
                       v1.X * v2.Y - v2.X * v1.Y);
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
        return X * X + Y * Y + Z * Z;
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
        X *= oneOverLength;
        Y *= oneOverLength;
        Z *= oneOverLength;
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
        return Vector3(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);
    }

    inline Vector3& operator+=(Vector3& v1, const Vector3& v2)
    {
        v1 = v1 + v2;
        return v1;
    }

    inline Vector3 operator-(const Vector3& v1, const Vector3& v2)
    {
        return Vector3(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z);
    }

    inline Vector3& operator-=(Vector3& v1, const Vector3& v2)
    {
        v1 = v1 - v2;
        return v1;
    }

    inline Vector3 operator*(const Vector3& v, const float& s)
    {
        return Vector3(v.X * s, v.Y * s, v.Z * s);
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

