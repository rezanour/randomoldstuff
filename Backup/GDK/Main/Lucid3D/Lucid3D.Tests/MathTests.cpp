#include "StdAfx.h"

using namespace Lucid3D;

namespace Lucid3D
{
    namespace Tests
    {
        bool BasicVectorTests();
        bool BasicMatrixTests();
        bool AdvancedMatrixTests();

        SubTest mathSubTests[] =
        {
            DefineSubTest(BasicVectorTests),
            DefineSubTest(BasicMatrixTests),
            DefineSubTest(AdvancedMatrixTests),
        };

        bool BasicVectorTests()
        {
            // This doesn't really test anything yet, just shows various usage patterns (and ensures they all compile)

            Vector3 v;
            v.x = 3;
            v.y = 4;
            v.z = 5;
            v.SetX(1).SetY(2).SetZ(3);

            Vector3 v1(v);
            v1.Set(4, 5, 6);

            Vector3 v2 = v.Cross(v1.SetY(144));

            float d = v2.Dot(v1);
            v2 = Vector3::Cross(v, v1);

            float f = Vector3::Dot(v2, v);

            if (v1.Normalize().Length() != 1)
                return false;

            Vector3 v3 = Vector3::Forward();
            Vector3 v4 = v1 + v2;
            Vector3 v5 = v4 - v;
            v3 += v2;
            v3 -= v;

            v4 = v5 * f;
            v3 = d * v4;
            v2 *= f;
            v *= d;

            const Vector2 vec2;

            return true;
        }

        bool BasicMatrixTests()
        {
            // This doesn't really test anything yet, just shows various usage patterns (and ensures they all compile)

            Matrix m;

            m.SetRow(3, Vector3::Forward(), 0.0f);

            Matrix m2(m);

            Matrix m3(Matrix::Transpose(m2));

            m2 = m2 * m3;
            Vector3 v(3, 5, 0);

            Vector3 transformed(v * m3);

            v.Transform(m);

            m3.SetRow(3, v, 1.0f);

            return true;
        }

        bool AdvancedMatrixTests()
        {
            Matrix m(1, 0, 0, 0,
                     0, 2, 0, 0,
                     0, 0, 3, 0,
                     0, 0, 0, 4);

            float trace = m.Trace();
            float expectedTrace = 1 + 2 + 3 + 4;

            if (trace != expectedTrace)
            {
                wprintf(L"trace didn't match! %f\n", trace);
                return false;
            }

            float det = m.Determinant();
            // that's a non-reflective, scaling matrix with a scale > 1, so we should have a pos det > 1
            if (det <= 1)
            {
                wprintf(L"determinant wasn't > 1! %f\n", det);
                return false;
            }

            Matrix invM;
            // we only don't have an inverse if det(M) == 0
            if (m.Inverse(&invM) != (det != 0))
            {
                wprintf(L"inverse existence didn't match determinant! %f\n", det);
                return false;
            }

            // if we had an inverse, let's verify it
            if (det != 0)
            {
                // first, m * invM == I
                Matrix testForIdentity = m * invM;
                // quick test for identiy is trace == 4 && det = 1. Not perfect, but good enough
                if (testForIdentity.Trace() != 4 || testForIdentity.Determinant() != 1)
                {
                    wprintf(L"M * invM != I!\n");
                    return false;
                }

                // second, inverting the inverse should give us the original matrix
                Matrix testM;
                if (!invM.Inverse(&testM))
                {
                    return false;
                }

                if (testM.Trace() != trace || testM.Determinant() != det)
                {
                    wprintf(L"inverting the inverse didn't appear to give the original matrix!\n");
                    return false;
                }
            }

            // inverse of an orthonormal matrix (any rotation matrix) is equivelant to it's transpose
            Matrix m2(Matrix::CreateFromAxisAngle(Vector3(0.707f, 0.707f, 0.0f), 1.234f));
            Matrix invM2;
            if (!m2.Inverse(&invM2))
            {
                return false;
            }

            Matrix transM2 = m2.Transpose();
            if (invM2.Trace() != transM2.Trace() || invM2.Determinant() != transM2.Determinant())
            {
                return false;
            }

            // another inverse test
            Matrix m3(1, 2, 0, 0,
                      4, 0, 4, 0,
                      0, 8, 8, 0,
                      1, 0, 2, 1);
            Matrix invM3;
            if (!m3.Inverse(&invM3))
            {
                return false;
            }

            DirectX::XMMATRIX xm = m3;
            DirectX::XMVECTOR xdet;
            DirectX::XMMATRIX invXM = DirectX::XMMatrixInverse(&xdet, xm);

            DirectX::XMFLOAT4X4 invXMF;
            DirectX::XMStoreFloat4x4(&invXMF, invXM);

            return true;
        }

        bool MathTests()
        {
            return RunSubTests(__FUNCTION__, mathSubTests, _countof(mathSubTests));
        }
    }
}
