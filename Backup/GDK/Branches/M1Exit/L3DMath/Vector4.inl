// Implementation of Vector4
#pragma once

#include "Vector3.h"
#include "Matrix.h"
#include <math.h>

namespace L3DMath
{
    inline Vector4::Vector4()
        : X(0.0f), Y(0.0f), Z(0.0f), W(0.0f)
    {
    }

    inline Vector4::Vector4(float x, float y, float z, float w)
        : X(x), Y(y), Z(z), W(w)
    {
    }

    inline Vector4::Vector4(const Vector3& v, float w)
        : X(v.X), Y(v.Y), Z(v.Z), W(w)
    {
    }

    inline Vector4::Vector4(const Vector4& v)
        : X(v.X), Y(v.Y), Z(v.Z)
    {
    }

#ifdef INTEROP
    // implict cast constructors
    inline Vector4::Vector4(const DirectX::XMFLOAT4& xmf)
        : X(xmf.x), Y(xmf.y), Z(xmf.z), W(xmf.w)
    {
    }

    inline Vector4::Vector4(const DirectX::XMVECTOR& xmv)
    {
        DirectX::XMFLOAT4 f;
        DirectX::XMStoreFloat4(&f, xmv);
        *this = f;
    }

    // conversions
    inline Vector4::operator DirectX::XMFLOAT4() const
    {
        return DirectX::XMFLOAT4(X, Y, Z, W);
    }

    inline Vector4::operator DirectX::XMVECTOR() const
    {
        DirectX::XMFLOAT4 f(X, Y, Z, W);
        return DirectX::XMLoadFloat4(&f);
    }

    // assignment
    inline Vector4& Vector4::operator= (const DirectX::XMFLOAT4& xmf)
    {
        X = xmf.x;
        Y = xmf.y;
        Z = xmf.z;
        return *this;
    }

    inline Vector4& Vector4::operator= (const DirectX::XMVECTOR& xmv)
    {
        DirectX::XMFLOAT4 f;
        DirectX::XMStoreFloat4(&f, xmv);
        *this = f;
        return *this;
    }
#endif

    inline Vector4& Vector4::operator= (const Vector4& v)
    {
        X = v.X;
        Y = v.Y;
        Z = v.Z;
        W = v.W;
        return *this;
    }

    // Element Access
    inline Vector4& Vector4::Set(float x, float y, float z, float w)
    {
        X = x;
        Y = y;
        Z = z;
        W = w;
        return *this;
    }

    inline Vector4& Vector4::SetX(float x)
    {
        X = x;
        return *this;
    }

    inline Vector4& Vector4::SetY(float y)
    {
        Y = y;
        return *this;
    }

    inline Vector4& Vector4::SetZ(float z)
    {
        Z = z;
        return *this;
    }

    inline Vector4& Vector4::SetW(float w)
    {
        W = w;
        return *this;
    }

    inline float&   Vector4::operator[](unsigned int n)
    {
        return n == 0 ? X : (n == 1 ? Y : (n == 2 ? Z : W));
    }

    inline Vector3  Vector4::ToVector3() const
    {
        return Vector3(X, Y, Z);
    }
    
    // Operations
    inline float        Vector4::Dot(const Vector4& v1, const Vector4& v2)
    {
        return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z + v1.W * v2.W;
    }

    inline Vector4      Vector4::Normalize(const Vector4& v)
    {
        float oneOverLength = 1.0f / v.Length();
        return v * oneOverLength;
    }

    inline Vector4      Vector4::Transform(const Vector4& v, const Matrix& m)
    {
        return v * m;
    }

    inline float        Vector4::Length() const
    {
        return sqrt(LengthSquared());
    }

    inline float        Vector4::LengthSquared() const
    {
        return X * X + Y * Y + Z * Z + W * W;
    }

    inline float        Vector4::Dot(const Vector4& v)
    {
        return Dot(*this, v);
    }

    inline Vector4&     Vector4::Normalize()
    {
        float oneOverLength = 1.0f / Length();
        X *= oneOverLength;
        Y *= oneOverLength;
        Z *= oneOverLength;
        W *= oneOverLength;
        return *this;
    }

    inline Vector4&     Vector4::Transform(const Matrix& m)
    {
        *this = *this * m;
        return *this;
    }

    inline Vector4 Vector4::Zero()
    {
        return Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    inline Vector4 Vector4::One()
    {
        return Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    inline Vector4 Vector4::UnitX()
    {
        return Vector4(1.0f, 0.0f, 0.0f, 0.0f);
    }

    inline Vector4 Vector4::UnitY()
    {
        return Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    }

    inline Vector4 Vector4::UnitZ()
    {
        return Vector4(0.0f, 0.0f, 1.0f, 0.0f);
    }

    inline Vector4 Vector4::UnitW()
    {
        return Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    // negate
    inline Vector4 operator-(const Vector4& v)
    {
        return v * -1.0f;
    }

    // Addition, Subtraction, Scalar multiplication
    inline Vector4 operator+(const Vector4& v1, const Vector4& v2)
    {
        return Vector4(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z, v1.W + v2.W);
    }

    inline Vector4& operator+=(Vector4& v1, const Vector4& v2)
    {
        v1 = v1 + v2;
        return v1;
    }

    inline Vector4 operator-(const Vector4& v1, const Vector4& v2)
    {
        return Vector4(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z, v1.W - v2.W);
    }

    inline Vector4& operator-=(Vector4& v1, const Vector4& v2)
    {
        v1 = v1 - v2;
        return v1;
    }

    inline Vector4 operator*(const Vector4& v, const float& s)
    {
        return Vector4(v.X * s, v.Y * s, v.Z * s, v.W * s);
    }

    inline Vector4& operator*=(Vector4& v, const float& s)
    {
        v = v * s;
        return v;
    }

    inline Vector4 operator*(const float& s, const Vector4& v)
    {
        return v * s;
    }

}

