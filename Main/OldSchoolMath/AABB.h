#pragma once

OSMALIGN16
struct AABB
{
    // Constructors
    AABB() : _center(DirectX::XMVectorZero()), _extents(DirectX::XMVectorSet(0.5f, 0.5f, 0.5f, 0.5f))
    {}

    AABB(_In_ DirectX::FXMVECTOR center, _In_ DirectX::FXMVECTOR extents) : _center(center), _extents(extents)
    {}

    // Access
    DirectX::XMVECTOR OSMCALL GetCenterV() const
    {
        return _center;
    }

    // Distance from the center to each side (radius)
    DirectX::XMVECTOR OSMCALL GetExtentsV() const
    {
        return _extents;
    }

    DirectX::XMVECTOR OSMCALL GetMinV() const
    {
        return DirectX::XMVectorSubtract(_center, _extents);
    }

    DirectX::XMVECTOR OSMCALL GetMaxV() const
    {
        return DirectX::XMVectorAdd(_center, _extents);
    }

    DirectX::XMFLOAT3 OSMCALL GetCenter() const
    {
        DirectX::XMFLOAT3 v;
        DirectX::XMStoreFloat3(&v, _center);
        return v;
    }

    DirectX::XMFLOAT3 OSMCALL GetExtents() const
    {
        DirectX::XMFLOAT3 v;
        DirectX::XMStoreFloat3(&v, _extents);
        return v;
    }

    DirectX::XMFLOAT3 OSMCALL GetMin() const
    {
        DirectX::XMFLOAT3 v;
        DirectX::XMStoreFloat3(&v, GetMinV());
        return v;
    }

    DirectX::XMFLOAT3 OSMCALL GetMax() const
    {
        DirectX::XMFLOAT3 v;
        DirectX::XMStoreFloat3(&v, GetMaxV());
        return v;
    }

    void OSMCALL Set(_In_ DirectX::FXMVECTOR center, _In_ DirectX::FXMVECTOR extents)
    {
        _center = center;
        _extents = extents;
    }

    // Utility
    void OSMCALL AddMargin(_In_ float margin)
    {
        _extents = DirectX::XMVectorScale(_extents, 1.0f + margin);
    }

    void OSMCALL ExpandBy(_In_ DirectX::FXMVECTOR amount)
    {
        AABB swept(DirectX::XMVectorAdd(_center, amount), _extents);
        DirectX::XMVECTOR newCenter = DirectX::XMVectorScale(DirectX::XMVectorAdd(_center, swept._center), 0.5f);
        DirectX::XMVECTOR newExtents = DirectX::XMVectorSubtract(DirectX::XMVectorMax(GetMaxV(), swept.GetMaxV()), newCenter);
        Set(newCenter, newExtents);
    }

    bool OSMCALL Overlaps(_In_ const AABB& other)
    {
        DirectX::XMVECTOR dist = DirectX::XMVectorAbs(DirectX::XMVectorSubtract(_center, other._center));
        DirectX::XMVECTOR r = DirectX::XMVectorAdd(_extents, other._extents);
        return DirectX::XMVector3LessOrEqual(dist, r);
    }

    _Success_(return)
    bool OSMCALL Raycast(_In_ DirectX::FXMVECTOR start, _In_ DirectX::FXMVECTOR dir, _Out_ float& distance)
    {
        // Optimized SIMD raycast code from DirectXMath
        const DirectX::XMVECTOR RayEpsilon = DirectX::XMVectorSet(1e-20f, 1e-20f, 1e-20f, 1e-20f);
        const DirectX::XMVECTOR FltMin = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
        const DirectX::XMVECTOR FltMax = DirectX::XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

        // Adjust ray origin to be relative to center of the box.
        DirectX::XMVECTOR TOrigin = DirectX::XMVectorSubtract(_center, start);

        // Compute the dot product againt each axis of the box.
        // Since the axii are (1,0,0), (0,1,0), (0,0,1) no computation is necessary.
        DirectX::XMVECTOR AxisDotOrigin = TOrigin;
        DirectX::XMVECTOR AxisDotDirection = dir;

        // if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
        DirectX::XMVECTOR IsParallel = DirectX::XMVectorLessOrEqual(DirectX::XMVectorAbs(AxisDotDirection), RayEpsilon);

        // Test against all three axii simultaneously.
        DirectX::XMVECTOR InverseAxisDotDirection = DirectX::XMVectorReciprocal(AxisDotDirection);
        DirectX::XMVECTOR t1 = DirectX::XMVectorMultiply(DirectX::XMVectorSubtract(AxisDotOrigin, _extents), InverseAxisDotDirection);
        DirectX::XMVECTOR t2 = DirectX::XMVectorMultiply(DirectX::XMVectorAdd(AxisDotOrigin, _extents), InverseAxisDotDirection);

        // Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
        // use the results from any directions parallel to the slab.
        DirectX::XMVECTOR t_min = DirectX::XMVectorSelect(DirectX::XMVectorMin(t1, t2), FltMin, IsParallel);
        DirectX::XMVECTOR t_max = DirectX::XMVectorSelect(DirectX::XMVectorMax(t1, t2), FltMax, IsParallel);

        // t_min.x = maximum( t_min.x, t_min.y, t_min.z );
        // t_max.x = minimum( t_max.x, t_max.y, t_max.z );
        t_min = DirectX::XMVectorMax(t_min, DirectX::XMVectorSplatY(t_min));  // x = max(x,y)
        t_min = DirectX::XMVectorMax(t_min, DirectX::XMVectorSplatZ(t_min));  // x = max(max(x,y),z)
        t_max = DirectX::XMVectorMin(t_max, DirectX::XMVectorSplatY(t_max));  // x = min(x,y)
        t_max = DirectX::XMVectorMin(t_max, DirectX::XMVectorSplatZ(t_max));  // x = min(min(x,y),z)

        // if ( t_min > t_max ) return false;
        DirectX::XMVECTOR NoIntersection = DirectX::XMVectorGreater(DirectX::XMVectorSplatX(t_min), DirectX::XMVectorSplatX(t_max));

        // if ( t_max < 0.0f ) return false;
        NoIntersection = DirectX::XMVectorOrInt(NoIntersection, DirectX::XMVectorLess(DirectX::XMVectorSplatX(t_max), DirectX::XMVectorZero()));

        // if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return false;
        DirectX::XMVECTOR ParallelOverlap = DirectX::XMVectorInBounds(AxisDotOrigin, _extents);
        NoIntersection = DirectX::XMVectorOrInt(NoIntersection, DirectX::XMVectorAndCInt(IsParallel, ParallelOverlap));

        if (DirectX::XMVector3Equal(NoIntersection, DirectX::XMVectorZero()))
        {
            // Store the x-component to *pDist
            DirectX::XMStoreFloat(&distance, t_min);
            return true;
        }

        return false;
    }

private:
    DirectX::XMVECTOR _center;
    DirectX::XMVECTOR _extents;
};
