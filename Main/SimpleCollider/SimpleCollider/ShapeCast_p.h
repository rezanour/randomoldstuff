#pragma once

struct SupportMapping
{
    virtual XMVECTOR GetSupportPoint(FXMVECTOR direction) const = 0;
};

struct SphereSupportMapping : public SupportMapping
{
    XMVECTORF32 Center;
    float Radius;

    XMVECTOR GetSupportPoint(FXMVECTOR direction) const override
    {
        return direction * Radius;
    }
};

struct CubeSupportMapping : public SupportMapping
{
    XMVECTORF32 Center;
    XMVECTORF32 HalfWidths;

    XMVECTOR GetSupportPoint(FXMVECTOR direction) const override
    {
        XMVECTORF32 result;
        result.f[0] = Center.f[0] + XMVectorGetX(direction) >= 0 ? HalfWidths.f[0] : -HalfWidths.f[0];
        result.f[1] = Center.f[1] + XMVectorGetY(direction) >= 0 ? HalfWidths.f[1] : -HalfWidths.f[1];
        result.f[2] = Center.f[2] + XMVectorGetZ(direction) >= 0 ? HalfWidths.f[2] : -HalfWidths.f[2];
        result.f[3] = 1.0f;
        return result.v;
    }
};

struct TriangleSupportMapping : public SupportMapping
{
    XMVECTORF32 Points[3];

    XMVECTOR GetSupportPoint(FXMVECTOR direction) const override
    {
        uint32_t iMax = 0;
        XMVECTOR dMax = XMVector3Dot(Points[0], direction);
        XMVECTOR d = XMVector3Dot(Points[1], direction);
        if (XMVector2Greater(d, dMax))
        {
            dMax = d;
            iMax = 1;
        }
        d = XMVector3Dot(Points[2], direction);
        if (XMVector2Greater(d, dMax))
        {
            dMax = d;
            iMax = 2;
        }
        return Points[iMax];
    }
};

bool __vectorcall ShapeCast(const XMFLOAT3& posA, const XMFLOAT3& moveA, _In_ SupportMapping* supA,
                            const XMFLOAT3& posB, const XMFLOAT3& moveB, _In_ SupportMapping* supB,
                            _Out_ XMFLOAT3* normal, _Out_ float* distance);
