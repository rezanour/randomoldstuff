#include "Precomp.h"

//
// Implementation of Gino van den Bergen's GJK Raycast paper:
//    http://www.dtecta.com/papers/unpublished04raycast.pdf
//

static __forceinline void __vectorcall AddPoint(_In_reads_(5) XMVECTOR simplex[5], FXMVECTOR p, uint32_t* bits);
static __forceinline XMVECTOR __vectorcall FindSupportVectorAndReduce(_In_reads_(5) XMVECTOR simplex[5], FXMVECTOR x, uint32_t* bits);
static __forceinline void __vectorcall FindBarycentricCoordinates(FXMVECTOR a, FXMVECTOR b, FXMVECTOR c, FXMVECTOR p, _Out_ float* s, _Out_ float* r, _Out_ float* t);

_Use_decl_annotations_
bool __vectorcall ShapeCast(const XMFLOAT3& posA, const XMFLOAT3& moveA, SupportMapping* supA,
                            const XMFLOAT3& posB, const XMFLOAT3& moveB, SupportMapping* supB,
                            XMFLOAT3* normal, float* distance)
{
    XMVECTOR s = XMLoadFloat3(&posA);
    XMVECTOR r = XMVectorSubtract(XMLoadFloat3(&moveA), XMLoadFloat3(&moveB));

    XMVECTOR lambda = XMVectorZero();
    XMVECTOR x = s;
    XMVECTOR v = x - XMLoadFloat3(&posB);
    XMVECTOR vDotR;
    XMVECTOR p;
    XMVECTOR w;

    XMVECTOR simplexP[5] = {};
    uint32_t bits = 0;

    *distance = 0.0f;
    *normal = XMFLOAT3(0, 0, 0);

    XMVECTOR e2 = g_XMEpsilon * g_XMEpsilon;
    uint32_t iterations = 0;
    while (XMVector2Greater(XMVector3LengthSq(v), e2) && iterations++ < 1000)
    {
        v = XMVector3Normalize(v);
        p = supA->GetSupportPoint(v) + supB->GetSupportPoint(v);
        w = x - p;
        if (XMVector2Greater(XMVector3Dot(v, w), XMVectorZero()))
        {
            vDotR = XMVector3Dot(v, r);
            if (XMVector2GreaterOrEqual(vDotR, XMVectorZero()))
            {
                return false;
            }

            lambda = lambda - XMVector3Dot(v, w) / vDotR;
            if (XMVector2Greater(lambda, XMVectorSet(1, 1, 1, 1)))
            {
                return false;
            }

            x = s + lambda * r;
            XMStoreFloat3(normal, v);
        }

        AddPoint(simplexP, p, &bits);
        v = FindSupportVectorAndReduce(simplexP, x, &bits);
    }

    *distance = XMVectorGetX(lambda);

    return true;
}

_Use_decl_annotations_
__forceinline void __vectorcall AddPoint(XMVECTOR simplex[5], FXMVECTOR p, uint32_t* bits)
{
    uint32_t bit = 1;
    uint32_t i = 0;
    for (; i < 4; ++i, bit <<= 1)
    {
        if (((*bits) & bit) == 0)
        {
            *bits |= bit;
            break;
        }
    }
    simplex[i] = p;
}

_Use_decl_annotations_
__forceinline XMVECTOR __vectorcall FindSupportVectorAndReduce(XMVECTOR simplex[5], FXMVECTOR x, uint32_t* bits)
{
    uint32_t bit = 1;
    uint32_t i = 0;
    uint32_t bitCount = 0;
    XMVECTOR compact[4];
    uint32_t compactBits[4] = {};
    for (; i < 5; ++i, bit <<= 1)
    {
        if (((*bits) & bit) == bit)
        {
            compactBits[bitCount] = bit;
            compact[bitCount++] = simplex[i];
        }
    }

    XMVECTOR v;

    switch (bitCount)
    {
    case 0:     // Error, empty simplex!
        assert(false);
        return XMVectorZero();

    case 1:     // Point
        v = x - compact[0];
        break;

    case 2:     // Segment
    {
        XMVECTOR edge = compact[1] - compact[0];
        v = XMVector3Cross(edge, XMVector3Cross(x - compact[0], edge));
        break;
    }

    case 4:     // Tetrahedron
    {
        // Reduce. For each point, consider the triangle
        // formed by the other 3. If x & this point are on the same
        // side, then this point is valid. If x is on opposite side, we
        // drop this point
        XMVECTOR others[3];
        uint32_t count = 0;
        for (uint32_t i = 0; i < 4; ++i)
        {
            XMVECTOR p = compact[i];
            count = 0;
            for (uint32_t j = 0; j < 4; ++j)
            {
                if (i != j)
                {
                    others[count++] = compact[j];
                }
            }

            XMVECTOR edge0 = others[1] - others[0];
            XMVECTOR edge1 = others[2] - others[0];
            XMVECTOR n = XMVector3Cross(edge0, edge1);
            if (XMVector2LessOrEqual(XMVectorMultiply(XMVector3Dot(n, x - others[0]), XMVector3Dot(n, p - others[0])), XMVectorZero()))
            {
                (*bits) &= ~compactBits[i];
                for (uint32_t j = i; j < 3; ++j)
                {
                    compact[j] = compact[j + 1];
                }
                --bitCount;
                break;
            }
        }

        if (bitCount == 4)
        {
            // Couldn't reduce the simplex. Just drop the front most point and rotate
            simplex[0] = compact[0];
            simplex[1] = compact[1];
            simplex[2] = compact[2];
            *bits = 7;
        }

        __fallthrough;
    }

    case 3:     // Triangle
    {
        XMVECTOR edge0 = compact[1] - compact[0];
        XMVECTOR edge1 = compact[2] - compact[0];
        v = XMVector3Cross(edge0, edge1);
        if (XMVector2Less(XMVector3Dot(v, x - compact[0]), XMVectorZero()))
        {
            v = -v;
        }
        break;
    }

    default:
        assert(false);
        return XMVectorZero();
    }

    v = XMVector3Normalize(v);
    v *= XMVector3Dot(v, x - compact[0]);
    return v;
}

_Use_decl_annotations_
__forceinline void __vectorcall FindBarycentricCoordinates(FXMVECTOR a, FXMVECTOR b, FXMVECTOR c, FXMVECTOR p, float* s, float* r, float* t)
{
    XMVECTOR u = b - a;
    XMVECTOR v = c - a;
    XMVECTOR w = p - a;
    XMVECTOR vCrossW = XMVector3Cross(v, w);
    XMVECTOR vCrossU = XMVector3Cross(v, u);

    // Validate r is positive (should be if p is in triangle)
    assert(XMVector2GreaterOrEqual(XMVector3Dot(vCrossW, vCrossU), XMVectorZero()));

    XMVECTOR uCrossW = XMVector3Cross(u, w);
    XMVECTOR uCrossV = XMVector3Cross(u, v);

    // Validate t is positive (should be if p is in triangle)
    assert(XMVector2GreaterOrEqual(XMVector3Dot(uCrossW, uCrossV), XMVectorZero()));

    XMVECTOR denom = XMVector3Length(uCrossV);
    XMVECTOR R = XMVector3Length(vCrossW) / denom;
    XMVECTOR T = XMVector3Length(uCrossW) / denom;

    assert(XMVector2LessOrEqual(R + T, XMVectorSet(1, 1, 1, 1)));

    *r = XMVectorGetX(R);
    *t = XMVectorGetX(T);
    *s = 1 - (*r) - (*t);
}
