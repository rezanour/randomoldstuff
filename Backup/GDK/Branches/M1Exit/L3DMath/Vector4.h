// 4 dimensional vector
#pragma once

#ifdef XNAMATH_INTEROP
namespace DirectX
{
    #include <XnaMath.h>
}
#define INTEROP
#endif

#ifdef DIRECTXMATH_INTEROP
#include <DirectXTK\DirectxMath.h>
#define INTEROP
#endif

namespace L3DMath
{
    class Vector3;
    class Matrix;

    class Vector4
    {
    public:
        // Construction
        Vector4();
        Vector4(float x, float y, float z, float w);
        Vector4(const Vector3& v, float w);
        Vector4(const Vector4& v);

#ifdef INTEROP
        // implict cast constructors
        Vector4(const DirectX::XMFLOAT4& xmf);
        Vector4(const DirectX::XMVECTOR& xmv);

        // conversions
        operator DirectX::XMFLOAT4() const;
        operator DirectX::XMVECTOR() const;

        // assignment
        Vector4& operator= (const DirectX::XMFLOAT4& xmf);
        Vector4& operator= (const DirectX::XMVECTOR& xmv);
#endif

        // Assignment
        Vector4& operator= (const Vector4& v);

        // Element Access
        Vector4&    Set(float x, float y, float z, float w);
        Vector4&    SetX(float x);
        Vector4&    SetY(float y);
        Vector4&    SetZ(float z);
        Vector4&    SetW(float w);
        float& operator[](unsigned int n);

        Vector3     ToVector3() const;

        // Operations
        static float    Dot(const Vector4& v1, const Vector4& v2);
        static Vector4  Normalize(const Vector4& v);
        static Vector4  Transform(const Vector4& v, const Matrix& m);

        float           Length() const;
        float           LengthSquared() const;
        float           Dot(const Vector4& v);
        Vector4&        Normalize();
        Vector4&        Transform(const Matrix& m);

        // Static Vectors
        static Vector4  Zero();
        static Vector4  One();
        static Vector4  UnitX();
        static Vector4  UnitY();
        static Vector4  UnitZ();
        static Vector4  UnitW();

        float X, Y, Z, W;
    };

    // negate
    Vector4 operator-(const Vector4& v);

    // Addition, Subtraction, Scalar multiplication
    Vector4 operator+(const Vector4& v1, const Vector4& v2);
    Vector4& operator+=(Vector4& v1, const Vector4& v2);
    Vector4 operator-(const Vector4& v1, const Vector4& v2);
    Vector4& operator-=(Vector4& v1, const Vector4& v2);
    Vector4 operator*(const Vector4& v, const float& s);
    Vector4& operator*=(Vector4& v, const float& s);
    Vector4 operator*(const float& s, const Vector4& v);
}

#include "Vector4.inl"
