// Implementation of Vector3
#pragma once

#include <Lucid3D.Platform.h>
#include "Matrix.h"
#include <math.h>

namespace Lucid3D
{
    L3DINLINE Vector3::Vector3()
        : x(0.0f), y(0.0f), z(0.0f)
    {
    }

    L3DINLINE Vector3::Vector3(float x, float y, float z)
        : x(x), y(y), z(z)
    {
    }

    L3DINLINE Vector3::Vector3(const Vector3& v)
        : x(v.x), y(v.y), z(v.z)
    {
    }

#ifdef INTEROP
    // implict cast constructors
    L3DINLINE Vector3::Vector3(const DirectX::XMFLOAT3& xmf)
        : x(xmf.x), y(xmf.y), z(xmf.z)
    {
    }

    L3DINLINE Vector3::Vector3(const DirectX::XMVECTOR& xmv)
    {
        DirectX::XMFLOAT3 f;
        DirectX::XMStoreFloat3(&f, xmv);
        *this = f;
    }

    // conversions
    L3DINLINE Vector3::operator DirectX::XMFLOAT3() const
    {
        return DirectX::XMFLOAT3(x, y, z);
    }

    L3DINLINE Vector3::operator DirectX::XMVECTOR() const
    {
        DirectX::XMFLOAT3 f(x, y, z);
        return DirectX::XMLoadFloat3(&f);
    }

    // assignment
    L3DINLINE Vector3& Vector3::operator= (const DirectX::XMFLOAT3& xmf)
    {
        x = xmf.x;
        y = xmf.y;
        z = xmf.z;
        return *this;
    }

    L3DINLINE Vector3& Vector3::operator= (const DirectX::XMVECTOR& xmv)
    {
        DirectX::XMFLOAT3 f;
        DirectX::XMStoreFloat3(&f, xmv);
        *this = f;
        return *this;
    }
#endif

    L3DINLINE Vector3& Vector3::operator= (const Vector3& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    // Element Access
    L3DINLINE Vector3& Vector3::Set(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        return *this;
    }

    L3DINLINE Vector3& Vector3::SetX(float x)
    {
        this->x = x;
        return *this;
    }

    L3DINLINE Vector3& Vector3::SetY(float y)
    {
        this->y = y;
        return *this;
    }

    L3DINLINE Vector3& Vector3::SetZ(float z)
    {
        this->z = z;
        return *this;
    }

    L3DINLINE float&   Vector3::operator[](unsigned int n)
    {
        return n == 0 ? x : (n == 1 ? y : z);
    }
    
    // Operations
    L3DINLINE float        Vector3::Dot(const Vector3& v1, const Vector3& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    L3DINLINE Vector3      Vector3::Cross(const Vector3& v1, const Vector3& v2)
    {
        return Vector3(v1.y * v2.z - v2.y * v1.z, 
                       v1.z * v2.x - v2.z * v1.x,
                       v1.x * v2.y - v2.x * v1.y);
    }

    L3DINLINE Vector3      Vector3::Normalize(const Vector3& v)
    {
        float oneOverLength = 1.0f / v.Length();
        return v * oneOverLength;
    }

    L3DINLINE Vector3      Vector3::Transform(const Vector3& v, const Matrix& m)
    {
        return v * m;
    }

    L3DINLINE float        Vector3::Length() const
    {
        return sqrt(LengthSquared());
    }

    L3DINLINE float        Vector3::LengthSquared() const
    {
        return x * x + y * y + z * z;
    }

    L3DINLINE float        Vector3::Dot(const Vector3& v)
    {
        return Dot(*this, v);
    }

    L3DINLINE Vector3      Vector3::Cross(const Vector3& v)
    {
        return Cross(*this, v);
    }

    L3DINLINE Vector3&     Vector3::Normalize()
    {
        float oneOverLength = 1.0f / Length();
        x *= oneOverLength;
        y *= oneOverLength;
        z *= oneOverLength;
        return *this;
    }

    L3DINLINE Vector3&     Vector3::Transform(const Matrix& m)
    {
        *this = *this * m;
        return *this;
    }

    L3DINLINE Vector3 Vector3::Zero()
    {
        return Vector3(0.0f, 0.0f, 0.0f);
    }

    L3DINLINE Vector3 Vector3::One()
    {
        return Vector3(1.0f, 1.0f, 1.0f);
    }

    L3DINLINE Vector3 Vector3::UnitX()
    {
        return Vector3(1.0f, 0.0f, 0.0f);
    }

    L3DINLINE Vector3 Vector3::UnitY()
    {
        return Vector3(0.0f, 1.0f, 0.0f);
    }

    L3DINLINE Vector3 Vector3::UnitZ()
    {
        return Vector3(0.0f, 0.0f, 1.0f);
    }

    L3DINLINE Vector3 Vector3::Up()
    {
        return Vector3(0.0f, 1.0f, 0.0f);
    }

    L3DINLINE Vector3 Vector3::Down()
    {
        return Vector3(0.0f, -1.0f, 0.0f);
    }

    L3DINLINE Vector3 Vector3::Left()
    {
        return Vector3(-1.0f, 0.0f, 0.0f);
    }

    L3DINLINE Vector3 Vector3::Right()
    {
        return Vector3(1.0f, 0.0f, 0.0f);
    }

    L3DINLINE Vector3 Vector3::Forward()
    {
        return Vector3(0.0f, 0.0f, 1.0f);
    }

    L3DINLINE Vector3 Vector3::Backward()
    {
        return Vector3(0.0f, 0.0f, -1.0f);
    }

    // negate
    L3DINLINE Vector3 operator-(const Vector3& v)
    {
        return v * -1.0f;
    }

    // Addition, Subtraction, Scalar multiplication
    L3DINLINE Vector3 operator+(const Vector3& v1, const Vector3& v2)
    {
        return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    }

    L3DINLINE Vector3& operator+=(Vector3& v1, const Vector3& v2)
    {
        v1 = v1 + v2;
        return v1;
    }

    L3DINLINE Vector3 operator-(const Vector3& v1, const Vector3& v2)
    {
        return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }

    L3DINLINE Vector3& operator-=(Vector3& v1, const Vector3& v2)
    {
        v1 = v1 - v2;
        return v1;
    }

    L3DINLINE Vector3 operator*(const Vector3& v, const float& s)
    {
        return Vector3(v.x * s, v.y * s, v.z * s);
    }

    L3DINLINE Vector3& operator*=(Vector3& v, const float& s)
    {
        v = v * s;
        return v;
    }

    L3DINLINE Vector3 operator*(const float& s, const Vector3& v)
    {
        return v * s;
    }

}

