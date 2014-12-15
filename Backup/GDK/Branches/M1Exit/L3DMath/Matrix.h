// standard 4x4 homogenous affine matrix
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
    class Vector4;

    class Matrix
    {
    public:
        // Construction
        Matrix();
        Matrix(const Matrix& m);
        Matrix(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3);
        Matrix(const float values[16]);
        Matrix( float m00, float m01, float m02, float m03,
                float m10, float m11, float m12, float m13,
                float m20, float m21, float m22, float m23,
                float m30, float m31, float m32, float m33);

#ifdef INTEROP
        // implict cast constructors
        Matrix(const DirectX::XMFLOAT4X4& xmf);
        Matrix(const DirectX::XMMATRIX& xmv);

        // conversions
        operator DirectX::XMFLOAT4X4() const;
        operator DirectX::XMMATRIX() const;

        // assignment
        Matrix& operator= (const DirectX::XMFLOAT4X4& xmf);
        Matrix& operator= (const DirectX::XMMATRIX& xmv);
#endif

        // Assignment
        Matrix& operator= (const Matrix& m);

        // Element Access
        Matrix&         SetRow(unsigned int row, float x, float y, float z, float w);
        Matrix&         SetRow(unsigned int row, const Vector3& v, float w);
        Matrix&         SetRow(unsigned int row, const Vector4& v);

        Matrix&         SetColumn(unsigned int column, float x, float y, float z, float w);
        Matrix&         SetColumn(unsigned int column, const Vector3& v, float w);
        Matrix&         SetColumn(unsigned int column, const Vector4& v);

        float&          operator[](unsigned int n);

        float&          Get(unsigned int row, unsigned int column);

        Vector4         GetRow(unsigned int row) const;
        Vector4         GetColumn(unsigned int column) const;

        // Operations
        static Matrix   Multiply(const Matrix& m1, const Matrix& m2);
        static Matrix   Transpose(const Matrix& m1);

        Matrix          Multiply(const Matrix& m);
        Matrix&         Transpose();

        // Static Vectors
        static Matrix   Zero();
        static Matrix   Identity();

        union
        {
            float m[4][4];

            struct
            {
                float   m00, m01, m02, m03,
                        m10, m11, m12, m13,
                        m20, m21, m22, m23,
                        m30, m31, m32, m33;
            };
        };
    };

    // negate
    Matrix operator-(const Matrix& m);

    // Addition, Subtraction, Scalar multiplication
    Matrix operator+(const Matrix& m1, const Matrix& m2);
    Matrix& operator+=(Matrix& m, const Matrix& m2);
    Matrix operator-(const Matrix& m1, const Matrix& m2);
    Matrix& operator-=(Matrix& m1, const Matrix& m2);
    Matrix operator*(const Matrix& m, const float& s);
    Matrix operator*(const Matrix& m1, const Matrix& m2);
    Matrix& operator*=(Matrix& m1, const Matrix& m2);
    Matrix& operator*=(Matrix& m, const float& s);
    Matrix operator*(const float& s, const Matrix& m);
    Vector3 operator*(const Vector3& v, const Matrix& m);
    Vector4 operator*(const Vector4& v, const Matrix& m);
}

#include "Matrix.inl"
