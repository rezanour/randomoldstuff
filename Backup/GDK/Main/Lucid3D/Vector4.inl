// Implementation of Vector4
#pragma once

#include <Lucid3D\Lucid3D.Platform.h>
#include "Vector3.h"
#include "Matrix.h"
#include <math.h>

namespace Lucid3D
{
    L3DINLINE Vector4::Vector4()
        : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
    {
    }

    L3DINLINE Vector4::Vector4(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w)
    {
    }

    L3DINLINE Vector4::Vector4(const Vector3& v, float w)
        : x(v.x), y(v.y), z(v.z), w(w)
    {
    }

    L3DINLINE Vector4::Vector4(const Vector4& v)
        : x(v.x), y(v.y), z(v.z)
    {
    }

#ifdef INTEROP
    // implict cast constructors
    L3DINLINE Vector4::Vector4(const DirectX::XMFLOAT4& xmf)
        : x(xmf.x), y(xmf.y), z(xmf.z), w(xmf.w)
    {
    }

    L3DINLINE Vector4::Vector4(const DirectX::XMVECTOR& xmv)
    {
        DirectX::XMFLOAT4 f;
        DirectX::XMStoreFloat4(&f, xmv);
        *this = f;
    }

    // conversions
    L3DINLINE Vector4::operator DirectX::XMFLOAT4() const
    {
        return DirectX::XMFLOAT4(x, y, z, w);
    }

    L3DINLINE Vector4::operator DirectX::XMVECTOR() const
    {
        DirectX::XMFLOAT4 f(x, y, z, w);
        return DirectX::XMLoadFloat4(&f);
    }

    // assignment
    L3DINLINE Vector4& Vector4::operator= (const DirectX::XMFLOAT4& xmf)
    {
        x = xmf.x;
        y = xmf.y;
        z = xmf.z;
        return *this;
    }

    L3DINLINE Vector4& Vector4::operator= (const DirectX::XMVECTOR& xmv)
    {
        DirectX::XMFLOAT4 f;
        DirectX::XMStoreFloat4(&f, xmv);
        *this = f;
        return *this;
    }
#endif

    L3DINLINE Vector4& Vector4::operator= (const Vector4& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    // Element Access
    L3DINLINE Vector4& Vector4::Set(float x, float y, float z, float w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
        return *this;
    }

    L3DINLINE Vector4& Vector4::SetX(float x)
    {
        this->x = x;
        return *this;
    }

    L3DINLINE Vector4& Vector4::SetY(float y)
    {
        this->y = y;
        return *this;
    }

    L3DINLINE Vector4& Vector4::SetZ(float z)
    {
        this->z = z;
        return *this;
    }

    L3DINLINE Vector4& Vector4::SetW(float w)
    {
        this->w = w;
        return *this;
    }

    L3DINLINE float&   Vector4::operator[](unsigned int n)
    {
        return n == 0 ? x : (n == 1 ? y : (n == 2 ? z : w));
    }

    L3DINLINE Vector3  Vector4::ToVector3() const
    {
        return Vector3(x, y, z);
    }
    
    // Operations
    L3DINLINE float        Vector4::Dot(const Vector4& v1, const Vector4& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
    }

    L3DINLINE Vector4      Vector4::Normalize(const Vector4& v)
    {
        float oneOverLength = 1.0f / v.Length();
        return v * oneOverLength;
    }

    L3DINLINE Vector4      Vector4::Transform(const Vector4& v, const Matrix& m)
    {
        return v * m;
    }

    L3DINLINE float        Vector4::Length() const
    {
        return sqrt(LengthSquared());
    }

    L3DINLINE float        Vector4::LengthSquared() const
    {
        return x * x + y * y + z * z + w * w;
    }

    L3DINLINE float        Vector4::Dot(const Vector4& v)
    {
        return Dot(*this, v);
    }

    L3DINLINE Vector4&     Vector4::Normalize()
    {
        float oneOverLength = 1.0f / Length();
        x *= oneOverLength;
        y *= oneOverLength;
        z *= oneOverLength;
        w *= oneOverLength;
        return *this;
    }

    L3DINLINE Vector4&     Vector4::Transform(const Matrix& m)
    {
        *this = *this * m;
        return *this;
    }

    L3DINLINE Vector4 Vector4::Zero()
    {
        return Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    L3DINLINE Vector4 Vector4::One()
    {
        return Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    L3DINLINE Vector4 Vector4::UnitX()
    {
        return Vector4(1.0f, 0.0f, 0.0f, 0.0f);
    }

    L3DINLINE Vector4 Vector4::UnitY()
    {
        return Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    }

    L3DINLINE Vector4 Vector4::UnitZ()
    {
        return Vector4(0.0f, 0.0f, 1.0f, 0.0f);
    }

    L3DINLINE Vector4 Vector4::UnitW()
    {
        return Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    // negate
    L3DINLINE Vector4 operator-(const Vector4& v)
    {
        return v * -1.0f;
    }

    // Addition, Subtraction, Scalar multiplication
    L3DINLINE Vector4 operator+(const Vector4& v1, const Vector4& v2)
    {
        return Vector4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
    }

    L3DINLINE Vector4& operator+=(Vector4& v1, const Vector4& v2)
    {
        v1 = v1 + v2;
        return v1;
    }

    L3DINLINE Vector4 operator-(const Vector4& v1, const Vector4& v2)
    {
        return Vector4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
    }

    L3DINLINE Vector4& operator-=(Vector4& v1, const Vector4& v2)
    {
        v1 = v1 - v2;
        return v1;
    }

    L3DINLINE Vector4 operator*(const Vector4& v, const float& s)
    {
        return Vector4(v.x * s, v.y * s, v.z * s, v.w * s);
    }

    L3DINLINE Vector4& operator*=(Vector4& v, const float& s)
    {
        v = v * s;
        return v;
    }

    L3DINLINE Vector4 operator*(const float& s, const Vector4& v)
    {
        return v * s;
    }

}

