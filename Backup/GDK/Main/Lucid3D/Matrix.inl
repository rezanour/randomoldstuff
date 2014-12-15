// Implementation of Matrix 
#pragma once

#include <Lucid3D\Lucid3D.Platform.h>
#include "Vector4.h"
#include <stdlib.h>

namespace Lucid3D
{
    // Constructors
    L3DINLINE Matrix::Matrix()
    {
        *this = Matrix::Identity();
    }

    L3DINLINE Matrix::Matrix(const Matrix& m)
    {
        *this = m;
    }

    L3DINLINE Matrix::Matrix(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3)
    {
        SetRow(0, row0);
        SetRow(1, row1);
        SetRow(2, row2);
        SetRow(3, row3);
    }

    L3DINLINE Matrix::Matrix(const float values[16])
    {
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                m[r][c] = values[r * 4 + c];
    }

    L3DINLINE Matrix::Matrix(   float m00, float m01, float m02, float m03,
                                float m10, float m11, float m12, float m13,
                                float m20, float m21, float m22, float m23,
                                float m30, float m31, float m32, float m33)
    {
        m[0][0] = m00;  m[0][1] = m01;  m[0][2] = m02;  m[0][3] = m03;
        m[1][0] = m10;  m[1][1] = m11;  m[1][2] = m12;  m[1][3] = m13;
        m[2][0] = m20;  m[2][1] = m21;  m[2][2] = m22;  m[2][3] = m23;
        m[3][0] = m30;  m[3][1] = m31;  m[3][2] = m32;  m[3][3] = m33;
    }

    // Creation helpers
    L3DINLINE Matrix Matrix::CreateTransform(const Vector3& position, const Vector4& orientation, const Vector3& scale)
    {
        Matrix m(CreateFromQuaternion(orientation));
        m.SetRow(0, m.GetRow(0) * scale.x);
        m.SetRow(1, m.GetRow(1) * scale.y);
        m.SetRow(2, m.GetRow(2) * scale.z);
        m.SetRow(3, position, 1);
        return m;
    }

    L3DINLINE Matrix Matrix::CreateTranslation(const Vector3& position)
    {
        return Matrix(Vector4::UnitX(),
                      Vector4::UnitY(),
                      Vector4::UnitZ(),
                      Vector4(position, 1.0f));
    }

    L3DINLINE Matrix Matrix::CreateScale(const Vector3& scale)
    {
        return Matrix(Vector4(scale.x, 0, 0, 1),
                      Vector4(0, scale.y, 0, 1),
                      Vector4(0, 0, scale.z, 1),
                      Vector4::UnitW());
    }

    L3DINLINE Matrix Matrix::CreateFromQuaternion(const Vector4& quaternion)
    {
        // TODO: implement
        UNREFERENCED_PARAMETER(quaternion);
        return Matrix::Identity();
    }

    L3DINLINE Matrix Matrix::CreateFromAxisAngle(const Vector3& axis, float radians)
    {
        // TODO: implement
        UNREFERENCED_PARAMETER(axis);
        UNREFERENCED_PARAMETER(radians);
        return Matrix::Identity();
    }

    L3DINLINE Matrix Matrix::CreateRotationX(float radians)
    {
        return CreateFromAxisAngle(Vector3::Right(), radians);
    }

    L3DINLINE Matrix Matrix::CreateRotationY(float radians)
    {
        return CreateFromAxisAngle(Vector3::Up(), radians);
    }

    L3DINLINE Matrix Matrix::CreateRotationZ(float radians)
    {
        return CreateFromAxisAngle(Vector3::Forward(), radians);
    }

    L3DINLINE Matrix Matrix::CreateLookAt(const Vector3& cameraPosition, const Vector3& target, const Vector3& up)
    {
        // TODO: implement
        UNREFERENCED_PARAMETER(cameraPosition);
        UNREFERENCED_PARAMETER(target);
        UNREFERENCED_PARAMETER(up);
        return Matrix::Identity();
    }

    L3DINLINE Matrix Matrix::CreatePerspectiveFov(float fov, float aspectRatio, float nearClip, float farClip)
    {
        // TODO: implement
        UNREFERENCED_PARAMETER(fov);
        UNREFERENCED_PARAMETER(aspectRatio);
        UNREFERENCED_PARAMETER(nearClip);
        UNREFERENCED_PARAMETER(farClip);
        return Matrix::Identity();
    }

#ifdef INTEROP
    // implict cast constructors
    L3DINLINE Matrix::Matrix(const DirectX::XMFLOAT4X4& xmf)
    {
        *this = xmf;
    }

    L3DINLINE Matrix::Matrix(const DirectX::XMMATRIX& xmv)
    {
        DirectX::XMFLOAT4X4 f;
        DirectX::XMStoreFloat4x4(&f, xmv);
        *this = f;
    }

    // conversions
    L3DINLINE Matrix::operator DirectX::XMFLOAT4X4() const
    {
        return DirectX::XMFLOAT4X4((float*)m);
    }

    L3DINLINE Matrix::operator DirectX::XMMATRIX() const
    {
        DirectX::XMFLOAT4X4 f((float*)m);
        return DirectX::XMLoadFloat4x4(&f);
    }

    // assignment
    L3DINLINE Matrix& Matrix::operator= (const DirectX::XMFLOAT4X4& xmf)
    {
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                m[r][c] = xmf.m[r][c];

        return *this;
    }

    L3DINLINE Matrix& Matrix::operator= (const DirectX::XMMATRIX& xmv)
    {
        DirectX::XMFLOAT4X4 f;
        DirectX::XMStoreFloat4x4(&f, xmv);
        *this = f;
        return *this;
    }
#endif

    // Assignment
    L3DINLINE Matrix& Matrix::operator= (const Matrix& m)
    {
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                this->m[r][c] = m.m[r][c];

        return *this;
    }

    // Element Access
    L3DINLINE Matrix&     Matrix::SetRow(unsigned int row, float x, float y, float z, float w)
    {
        if (row > 3) row = 3;

        m[row][0] = x;
        m[row][1] = y;
        m[row][2] = z;
        m[row][3] = w;

        return *this;
    }

    L3DINLINE Matrix&     Matrix::SetRow(unsigned int row, const Vector3& v, float w)
    {
        if (row > 3) row = 3;

        m[row][0] = v.x;
        m[row][1] = v.y;
        m[row][2] = v.z;
        m[row][3] = w;

        return *this;
    }

    L3DINLINE Matrix&     Matrix::SetRow(unsigned int row, const Vector4& v)
    {
        if (row > 3) row = 3;

        m[row][0] = v.x;
        m[row][1] = v.y;
        m[row][2] = v.z;
        m[row][3] = v.w;

        return *this;
    }

    L3DINLINE Matrix&     Matrix::SetColumn(unsigned int column, float x, float y, float z, float w)
    {
        if (column > 3) column = 3;

        m[0][column] = x;
        m[1][column] = y;
        m[2][column] = z;
        m[3][column] = w;

        return *this;
    }

    L3DINLINE Matrix&     Matrix::SetColumn(unsigned int column, const Vector3& v, float w)
    {
        if (column > 3) column = 3;

        m[0][column] = v.x;
        m[1][column] = v.y;
        m[2][column] = v.z;
        m[3][column] = w;

        return *this;
    }

    L3DINLINE Matrix&     Matrix::SetColumn(unsigned int column, const Vector4& v)
    {
        if (column > 3) column = 3;

        m[0][column] = v.x;
        m[1][column] = v.y;
        m[2][column] = v.z;
        m[3][column] = v.w;

        return *this;
    }

    L3DINLINE float&      Matrix::operator[](unsigned int n)
    {
        if (n > 15) n = 15;

        return m[n / 4][n % 4];
    }

    L3DINLINE float&      Matrix::Get(unsigned int row, unsigned int column)
    {
        if (row > 3) row = 3;
        if (column > 3) column = 3;

        return m[row][column];
    }

    L3DINLINE float       Matrix::Get(unsigned int row, unsigned int column) const
    {
        if (row > 3) row = 3;
        if (column > 3) column = 3;

        return m[row][column];
    }

    L3DINLINE void        Matrix::Set(unsigned int row, unsigned int column, float value)
    {
        if (row > 3) row = 3;
        if (column > 3) column = 3;

        m[row][column] = value;
    }

    L3DINLINE Vector4     Matrix::GetRow(unsigned int row) const
    {
        if (row > 3) row = 3;

        return Vector4(m[row][0], m[row][1], m[row][2], m[row][3]);
    }

    L3DINLINE Vector4     Matrix::GetColumn(unsigned int column) const
    {
        if (column > 3) column =3;

        return Vector4(m[0][column], m[1][column], m[2][column], m[3][column]);
    }


    // Operations
    L3DINLINE Matrix   Matrix::Multiply(const Matrix& m1, const Matrix& m2)
    {
        Matrix result;

        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                result.m[r][c] = Vector4::Dot(m1.GetRow(r), m2.GetColumn(c));

        return result;
    }

    L3DINLINE Matrix   Matrix::Transpose(const Matrix& m1)
    {
        Matrix result;

        for (int i = 0; i < 4; i++)
            result.SetRow(i, m1.GetColumn(i));

        return result;
    }

    L3DINLINE float    Det2x2(float m00, float m01, 
                              float m10, float m11)
    {
        return m00 * m11 - m01 * m10;
    }

    L3DINLINE float    Det3x3(float m00, float m01, float m02, 
                              float m10, float m11, float m12, 
                              float m20, float m21, float m22)
    {
        return m00 * Det2x2(m11, m12, m21, m22) - m01 * Det2x2(m10, m12, m20, m22) + m02 * Det2x2(m10, m11, m20, m21);
    }

    L3DINLINE float    Matrix::Determinant(const Matrix& m)
    {
        return 
              m.m00 * Det3x3(m.m11, m.m12, m.m13, 
                             m.m21, m.m22, m.m23, 
                             m.m31, m.m32, m.m33)
            - m.m01 * Det3x3(m.m10, m.m12, m.m13,
                             m.m20, m.m22, m.m23,
                             m.m30, m.m32, m.m33)
            + m.m02 * Det3x3(m.m10, m.m11, m.m13,
                             m.m20, m.m21, m.m23,
                             m.m30, m.m31, m.m33)
            - m.m03 * Det3x3(m.m10, m.m11, m.m12,
                             m.m20, m.m21, m.m22,
                             m.m30, m.m31, m.m32);
    }

    L3DINLINE float    Matrix::Trace(const Matrix& m)
    {
        return m.m00 + m.m11 + m.m22 + m.m33;
    }

    L3DINLINE bool     Matrix::Inverse(const Matrix& m, Matrix* inverse)
    {
        // To implement Gaussian elimination, we do the following:
        // 1. To simulate the full augmented matrix, we track two side-by-side matrices
        //      The left side is a copy of the current matrix
        //      The right side starts off as identity
        // 2. As we work to make the left side equal to the identity (and ensure
        //      we perform all operations on both matrices), the right hand
        //      matrix will result in our inverse.
        // 3. NOTE: if we cared about the determinant, we could trace the determinant
        //      as we go as the number starting at 1, multiplied by each pivot element, and 
        //      multiplied by -1 for each row swap

        if (!inverse)
            return false;

        const float epsilon = 0.000001f;

        Matrix left = m;
        Matrix right = *inverse = Matrix::Identity();

        // forward pass of the Gaussian
        for (int i = 0; i < 4; i++)
        {
            // starting from ith row, find the row with the largest value in the ith column
            // this is our pivot element
            int swapRow = i;
            for (int row = i + 1; row < 4; row++)
            {
                if(abs(left.Get(row, i)) > abs(left.Get(swapRow, i)))
                    swapRow = row;
            }

            // if largest value is 0, we have 0 determinant and no inverse
            if (abs(left.Get(swapRow, i)) < epsilon)
            {
                return false;
            }

            // swap rows if needed, remembering to do it for the right matrix as well
            if (swapRow != i)
            {
                Vector4 temp = left.GetRow(i);
                left.SetRow(i, left.GetRow(swapRow));
                left.SetRow(swapRow, temp);

                temp = right.GetRow(i);
                right.SetRow(i, right.GetRow(swapRow));
                right.SetRow(swapRow, temp);
            }

            // scale the row so that ith column is 1
            float pivot = left.Get(i, i);
            if (pivot != 1.0f)
            {
                float scale = 1.0f / pivot;

                left.ScaleRow(i, scale);
                right.ScaleRow(i, scale);
            }

            // now, for each row j below i, subtract ith row scaled by ith column of jth row
            for (int j = i + 1; j < 4; j++)
            {
                Vector4 other = left.GetRow(i) * left.Get(j, i);
                left.SetRow(j, left.GetRow(j) - other);
                right.SetRow(j, right.GetRow(j) - other);
            }
        }

        // backwards pass of full Gaussian
        for (int i = 3; i >= 0; i--)
        {
            // now, for each row j above i, subtract ith row scaled by ith column of jth row
            for (int j = i - 1; j >= 0; j--)
            {
                Vector4 other = left.GetRow(i) * left.Get(j, i);
                left.SetRow(j, left.GetRow(j) - other);
                right.SetRow(j, right.GetRow(j) - other);
            }
        }

        *inverse = right;
        return true;
    }

    L3DINLINE Matrix          Matrix::Multiply(const Matrix& m)
    {
        return Matrix::Multiply(*this, m);
    }

    L3DINLINE Matrix&         Matrix::Transpose()
    {
        *this = Matrix::Transpose(*this);
        return *this;
    }

    L3DINLINE float           Matrix::Determinant()
    {
        return Determinant(*this);
    }

    L3DINLINE float           Matrix::Trace()
    {
        return Trace(*this);
    }

    L3DINLINE bool            Matrix::Inverse(Matrix* inverse)
    {
        return Inverse(*this, inverse);
    }

    L3DINLINE Matrix&         Matrix::ScaleRow(unsigned int row, float s)
    {
        return SetRow(row, GetRow(row) * s);
    }

    L3DINLINE Matrix&         Matrix::ScaleColumn(unsigned int column, float s)
    {
        return SetColumn(column, GetColumn(column) * s);
    }

    // Static Matrices
    L3DINLINE Matrix Matrix::Zero()
    {
        return Matrix(  Vector4::Zero(), 
                        Vector4::Zero(), 
                        Vector4::Zero(), 
                        Vector4::Zero());
    }

    L3DINLINE Matrix Matrix::Identity()
    {
        return Matrix(  Vector4::UnitX(), 
                        Vector4::UnitY(), 
                        Vector4::UnitZ(), 
                        Vector4::UnitW());
    }

    // negate
    L3DINLINE Matrix operator-(const Matrix& m)
    {
        return m * -1;
    }


    // Addition, Subtraction, Scalar multiplication
    L3DINLINE Matrix operator+(const Matrix& m1, const Matrix& m2)
    {
        return Matrix(  m1.GetRow(0) + m2.GetRow(0), 
                        m1.GetRow(1) + m2.GetRow(1),
                        m1.GetRow(2) + m2.GetRow(2),
                        m1.GetRow(3) + m2.GetRow(3));
    }

    L3DINLINE Matrix& operator+=(Matrix& m1, const Matrix& m2)
    {
        m1 = m1 + m2;
        return m1;
    }

    L3DINLINE Matrix operator-(const Matrix& m1, const Matrix& m2)
    {
        return Matrix(  m1.GetRow(0) - m2.GetRow(0), 
                        m1.GetRow(1) - m2.GetRow(1),
                        m1.GetRow(2) - m2.GetRow(2),
                        m1.GetRow(3) - m2.GetRow(3));
    }

    L3DINLINE Matrix& operator-=(Matrix& m1, const Matrix& m2)
    {
        m1 = m1 - m2;
        return m1;
    }

    L3DINLINE Matrix operator*(const Matrix& m1, const Matrix& m2)
    {
        return Matrix::Multiply(m1, m2);
    }

    L3DINLINE Matrix& operator*=(Matrix& m1, const Matrix& m2)
    {
        m1 = m1 * m2;
        return m1;
    }

    L3DINLINE Matrix operator*(const Matrix& m, const float& s)
    {
        Matrix result;

        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                result.m[r][c] = m.m[r][c] * s;

        return result;
    }

    L3DINLINE Matrix& operator*=(Matrix& m, const float& s)
    {
        m = m * s;
        return m;
    }

    L3DINLINE Matrix operator*(const float& s, const Matrix& m)
    {
        return m * s;
    }

    L3DINLINE Vector3 operator*(const Vector3& v, const Matrix& m)
    {
        return Vector3( Vector3::Dot(v, m.GetColumn(0).ToVector3()),
                        Vector3::Dot(v, m.GetColumn(1).ToVector3()),
                        Vector3::Dot(v, m.GetColumn(2).ToVector3()));
    }

    L3DINLINE Vector4 operator*(const Vector4& v, const Matrix& m)
    {
        return Vector4( Vector4::Dot(v, m.GetColumn(0)),
                        Vector4::Dot(v, m.GetColumn(1)),
                        Vector4::Dot(v, m.GetColumn(2)),
                        Vector4::Dot(v, m.GetColumn(3)));
    }
}

