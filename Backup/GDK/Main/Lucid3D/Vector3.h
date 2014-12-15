// 3 dimensional vector
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

namespace Lucid3D
{
    class Matrix;

    class Vector3
    {
    public:
        // Construction
        Vector3();
        Vector3(float x, float y, float z);
        Vector3(const Vector3& v);

#ifdef INTEROP
        // implict cast constructors
        Vector3(const DirectX::XMFLOAT3& xmf);
        Vector3(const DirectX::XMVECTOR& xmv);

        // conversions
        operator DirectX::XMFLOAT3 () const;
        operator DirectX::XMVECTOR () const;

        // assignment
        Vector3& operator= (const DirectX::XMFLOAT3& xmf);
        Vector3& operator= (const DirectX::XMVECTOR& xmv);
#endif

        // Assignment
        Vector3& operator= (const Vector3& v);

        // Element Access
        Vector3&    Set(float x, float y, float z);
        Vector3&    SetX(float x);
        Vector3&    SetY(float y);
        Vector3&    SetZ(float z);
        float& operator[](unsigned int n);

        // Operations
        static float    Dot(const Vector3& v1, const Vector3& v2);
        static Vector3  Cross(const Vector3& v1, const Vector3& v2);
        static Vector3  Normalize(const Vector3& v);
        static Vector3  Transform(const Vector3& v, const Matrix& m);

        float           Length() const;
        float           LengthSquared() const;
        float           Dot(const Vector3& v);
        Vector3         Cross(const Vector3& v);
        Vector3&        Normalize();
        Vector3&        Transform(const Matrix& m);

        // Static Vectors
        static Vector3  Zero();
        static Vector3  One();
        static Vector3  UnitX();
        static Vector3  UnitY();
        static Vector3  UnitZ();
        static Vector3  Up();
        static Vector3  Down();
        static Vector3  Left();
        static Vector3  Right();
        static Vector3  Forward();
        static Vector3  Backward();

        float x, y, z;
    };

    // negate
    Vector3 operator-(const Vector3& v);

    // Addition, Subtraction, Scalar multiplication
    Vector3 operator+(const Vector3& v1, const Vector3& v2);
    Vector3& operator+=(Vector3& v1, const Vector3& v2);
    Vector3 operator-(const Vector3& v1, const Vector3& v2);
    Vector3& operator-=(Vector3& v1, const Vector3& v2);
    Vector3 operator*(const Vector3& v, const float& s);
    Vector3& operator*=(Vector3& v, const float& s);
    Vector3 operator*(const float& s, const Vector3& v);
}

#include "Vector3.inl"
