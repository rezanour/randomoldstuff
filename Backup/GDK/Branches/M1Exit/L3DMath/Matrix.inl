// Implementation of Matrix 
#pragma once

#include "Vector4.h"
#include <stdlib.h>

namespace L3DMath
{
    inline Matrix::Matrix()
    {
        *this = Matrix::Identity();
    }

    inline Matrix::Matrix(const Matrix& m)
    {
        *this = m;
    }

    inline Matrix::Matrix(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3)
    {
        SetRow(0, row0);
        SetRow(1, row1);
        SetRow(2, row2);
        SetRow(3, row3);
    }

    inline Matrix::Matrix(const float values[16])
    {
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                m[r][c] = values[r * 4 + c];
    }

    inline Matrix::Matrix(  float m00, float m01, float m02, float m03,
                            float m10, float m11, float m12, float m13,
                            float m20, float m21, float m22, float m23,
                            float m30, float m31, float m32, float m33)
    {
        m[0][0] = m00;  m[0][1] = m01;  m[0][2] = m02;  m[0][3] = m03;
        m[1][0] = m10;  m[1][1] = m11;  m[1][2] = m12;  m[1][3] = m13;
        m[2][0] = m20;  m[2][1] = m21;  m[2][2] = m22;  m[2][3] = m23;
        m[3][0] = m30;  m[3][1] = m31;  m[3][2] = m32;  m[3][3] = m33;
    }

#ifdef INTEROP
    // implict cast constructors
    inline Matrix::Matrix(const DirectX::XMFLOAT4X4& xmf)
    {
        *this = xmf;
    }

    inline Matrix::Matrix(const DirectX::XMMATRIX& xmv)
    {
        DirectX::XMFLOAT4X4 f;
        DirectX::XMStoreFloat4x4(&f, xmv);
        *this = f;
    }

    // conversions
    inline Matrix::operator DirectX::XMFLOAT4X4() const
    {
        return DirectX::XMFLOAT4X4((float*)m);
    }

    inline Matrix::operator DirectX::XMMATRIX() const
    {
        DirectX::XMFLOAT4X4 f((float*)m);
        return DirectX::XMLoadFloat4x4(&f);
    }

    // assignment
    inline Matrix& Matrix::operator= (const DirectX::XMFLOAT4X4& xmf)
    {
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                m[r][c] = xmf.m[r][c];

        return *this;
    }

    inline Matrix& Matrix::operator= (const DirectX::XMMATRIX& xmv)
    {
        DirectX::XMFLOAT4X4 f;
        DirectX::XMStoreFloat4x4(&f, xmv);
        *this = f;
        return *this;
    }
#endif

    // Assignment
    inline Matrix& Matrix::operator= (const Matrix& m)
    {
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                this->m[r][c] = m.m[r][c];

        return *this;
    }

    // Element Access
    inline Matrix&     Matrix::SetRow(unsigned int row, float x, float y, float z, float w)
    {
        if (row > 3) row = 3;

        m[row][0] = x;
        m[row][1] = y;
        m[row][2] = z;
        m[row][3] = w;

        return *this;
    }

    inline Matrix&     Matrix::SetRow(unsigned int row, const Vector3& v, float w)
    {
        if (row > 3) row = 3;

        m[row][0] = v.X;
        m[row][1] = v.Y;
        m[row][2] = v.Z;
        m[row][3] = w;

        return *this;
    }

    inline Matrix&     Matrix::SetRow(unsigned int row, const Vector4& v)
    {
        if (row > 3) row = 3;

        m[row][0] = v.X;
        m[row][1] = v.Y;
        m[row][2] = v.Z;
        m[row][3] = v.W;

        return *this;
    }

    inline Matrix&     Matrix::SetColumn(unsigned int column, float x, float y, float z, float w)
    {
        if (column > 3) column = 3;

        m[0][column] = x;
        m[1][column] = y;
        m[2][column] = z;
        m[3][column] = w;

        return *this;
    }

    inline Matrix&     Matrix::SetColumn(unsigned int column, const Vector3& v, float w)
    {
        if (column > 3) column = 3;

        m[0][column] = v.X;
        m[1][column] = v.Y;
        m[2][column] = v.Z;
        m[3][column] = w;

        return *this;
    }

    inline Matrix&     Matrix::SetColumn(unsigned int column, const Vector4& v)
    {
        if (column > 3) column = 3;

        m[0][column] = v.X;
        m[1][column] = v.Y;
        m[2][column] = v.Z;
        m[3][column] = v.W;

        return *this;
    }

    inline float&      Matrix::operator[](unsigned int n)
    {
        if (n > 15) n = 15;

        return m[n / 4][n % 4];
    }


    inline float&      Matrix::Get(unsigned int row, unsigned int column)
    {
        if (row > 3) row = 3;
        if (column > 3) column = 3;

        return m[row][column];
    }


    inline Vector4     Matrix::GetRow(unsigned int row) const
    {
        if (row > 3) row = 3;

        return Vector4(m[row][0], m[row][1], m[row][2], m[row][3]);
    }

    inline Vector4     Matrix::GetColumn(unsigned int column) const
    {
        if (column > 3) column =3;

        return Vector4(m[0][column], m[1][column], m[2][column], m[3][column]);
    }


    // Operations
    inline Matrix   Matrix::Multiply(const Matrix& m1, const Matrix& m2)
    {
        Matrix result;

        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                result.m[r][c] = Vector4::Dot(m1.GetRow(r), m2.GetColumn(c));

        return result;
    }

    inline Matrix   Matrix::Transpose(const Matrix& m1)
    {
        Matrix result;

        for (int i = 0; i < 4; i++)
            result.SetRow(i, m1.GetColumn(i));

        return result;
    }

    inline Matrix          Matrix::Multiply(const Matrix& m)
    {
        return Matrix::Multiply(*this, m);
    }

    inline Matrix&         Matrix::Transpose()
    {
        *this = Matrix::Transpose(*this);
        return *this;
    }

    // Static Matrices
    inline Matrix Matrix::Zero()
    {
        return Matrix(  Vector4::Zero(), 
                        Vector4::Zero(), 
                        Vector4::Zero(), 
                        Vector4::Zero());
    }

    inline Matrix Matrix::Identity()
    {
        return Matrix(  Vector4::UnitX(), 
                        Vector4::UnitY(), 
                        Vector4::UnitZ(), 
                        Vector4::UnitW());
    }

    // negate
    inline Matrix operator-(const Matrix& m)
    {
        return m * -1;
    }


    // Addition, Subtraction, Scalar multiplication
    inline Matrix operator+(const Matrix& m1, const Matrix& m2)
    {
        return Matrix(  m1.GetRow(0) + m2.GetRow(0), 
                        m1.GetRow(1) + m2.GetRow(1),
                        m1.GetRow(2) + m2.GetRow(2),
                        m1.GetRow(3) + m2.GetRow(3));
    }

    inline Matrix& operator+=(Matrix& m1, const Matrix& m2)
    {
        m1 = m1 + m2;
        return m1;
    }

    inline Matrix operator-(const Matrix& m1, const Matrix& m2)
    {
        return Matrix(  m1.GetRow(0) - m2.GetRow(0), 
                        m1.GetRow(1) - m2.GetRow(1),
                        m1.GetRow(2) - m2.GetRow(2),
                        m1.GetRow(3) - m2.GetRow(3));
    }

    inline Matrix& operator-=(Matrix& m1, const Matrix& m2)
    {
        m1 = m1 - m2;
        return m1;
    }

    inline Matrix operator*(const Matrix& m1, const Matrix& m2)
    {
        return Matrix::Multiply(m1, m2);
    }

    inline Matrix& operator*=(Matrix& m1, const Matrix& m2)
    {
        m1 = m1 * m2;
        return m1;
    }

    inline Matrix operator*(const Matrix& m, const float& s)
    {
        Matrix result;

        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                result.m[r][c] = m.m[r][c] * s;

        return result;
    }

    inline Matrix& operator*=(Matrix& m, const float& s)
    {
        m = m * s;
        return m;
    }

    inline Matrix operator*(const float& s, const Matrix& m)
    {
        return m * s;
    }

    inline Vector3 operator*(const Vector3& v, const Matrix& m)
    {
        return Vector3( Vector3::Dot(v, m.GetColumn(0).ToVector3()),
                        Vector3::Dot(v, m.GetColumn(1).ToVector3()),
                        Vector3::Dot(v, m.GetColumn(2).ToVector3()));
    }

    inline Vector4 operator*(const Vector4& v, const Matrix& m)
    {
        return Vector4( Vector4::Dot(v, m.GetColumn(0)),
                        Vector4::Dot(v, m.GetColumn(1)),
                        Vector4::Dot(v, m.GetColumn(2)),
                        Vector4::Dot(v, m.GetColumn(3)));
    }
}

