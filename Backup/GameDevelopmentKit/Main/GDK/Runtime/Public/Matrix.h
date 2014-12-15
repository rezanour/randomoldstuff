#pragma once

// standard 4x4 homogenous affine matrix

namespace GDK
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

        // Creation helpers
        static Matrix CreateTransform(const Vector3& position, const Vector4& orientation, const Vector3& scale);

        static Matrix CreateTranslation(const Vector3& position);
        static Matrix CreateScale(const Vector3& scale);
        static Matrix CreateFromQuaternion(const Vector4& quaternion);
        static Matrix CreateFromAxisAngle(const Vector3& axis, float radians);
        static Matrix CreateRotationX(float radians);
        static Matrix CreateRotationY(float radians);
        static Matrix CreateRotationZ(float radians);

        static Matrix CreateLookAt(const Vector3& cameraPosition, const Vector3& target, const Vector3& up);
        static Matrix CreatePerspectiveFov(float fov, float aspectRatio, float nearClip, float farClip);

#ifdef DIRECTXMATH_INTEROP
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
        float           Get(unsigned int row, unsigned int column) const;
        void            Set(unsigned int row, unsigned int column, float value);

        Vector4         GetRow(unsigned int row) const;
        Vector4         GetColumn(unsigned int column) const;

        // Operations
        static Matrix   Multiply(const Matrix& m1, const Matrix& m2);
        static Matrix   Transpose(const Matrix& m1);

        // A basic determinant calculation. There are likely more optimal solutions
        static float    Determinant(const Matrix& m);
        static float    Trace(const Matrix& m);

        // A simple inverse using Gaussian-elimination.
        // TODO: Improve by using the 1/detM method
        static bool     Inverse(const Matrix& m, Matrix* inverse);

        Matrix          Multiply(const Matrix& m);
        Matrix&         Transpose();
        float           Determinant();
        float           Trace();
        bool            Inverse(Matrix* inverse);

        Matrix&         ScaleRow(unsigned int row, float s);
        Matrix&         ScaleColumn(unsigned int column, float s);

        // Static Matrices
        static Matrix   Zero();
        static Matrix   Identity();

        union
        {
            float m[4][4];

#pragma warning (push)
#pragma warning (disable: 4201) // we're using unnamed union, which is a warning. TODO: Investigate a better way later

            struct
            {
                float   m00, m01, m02, m03,
                        m10, m11, m12, m13,
                        m20, m21, m22, m23,
                        m30, m31, m32, m33;
            };

#pragma warning(pop)
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

#include <Matrix.inl>