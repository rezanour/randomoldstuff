#pragma once

//
// Adapted from Kasper Fauerby's paper.
// http://www.peroxide.dk
//

OSMALIGN16
struct CollisionQuery
{
    // Ellipsoid Radius
    DirectX::XMVECTOR eRadius;

    // Query (world space)
    DirectX::XMVECTOR WorldPosition;
    DirectX::XMVECTOR WorldMovement;

    // Query details (ellipsoid space)
    DirectX::XMVECTOR BasePoint;
    DirectX::XMVECTOR Velocity;
    DirectX::XMVECTOR VelocityLen2;

    // Best Result so far
    DirectX::XMVECTOR CollisionDistance;
    DirectX::XMVECTOR CollisionPoint;
    bool Collided;
};

// From the paper, this was the result of a competition for fastest impl :)
inline bool OSMCALL PointInTriangle(_In_ DirectX::FXMVECTOR p, _In_ DirectX::FXMVECTOR v0, _In_ DirectX::FXMVECTOR v1, _In_ DirectX::FXMVECTOR v2)
{
    DirectX::XMVECTOR e10 = DirectX::XMVectorSubtract(v1, v0);
    DirectX::XMVECTOR e20 = DirectX::XMVectorSubtract(v2, v0);

    DirectX::XMVECTOR a = DirectX::XMVector3Dot(e10, e10);
    DirectX::XMVECTOR b = DirectX::XMVector3Dot(e10, e20);
    DirectX::XMVECTOR c = DirectX::XMVector3Dot(e20, e20);

    float ac_bb = DirectX::XMVectorGetX(DirectX::XMVectorSubtract(DirectX::XMVectorMultiply(a, c), DirectX::XMVectorMultiply(b, b)));
    DirectX::XMVECTOR vp = DirectX::XMVectorSubtract(p, v0);

    DirectX::XMVECTOR d = DirectX::XMVector3Dot(vp, e10);
    DirectX::XMVECTOR e = DirectX::XMVector3Dot(vp, e20);

    float x = DirectX::XMVectorGetX(DirectX::XMVectorSubtract(DirectX::XMVectorMultiply(d, c), DirectX::XMVectorMultiply(e, b)));
    float y = DirectX::XMVectorGetX(DirectX::XMVectorSubtract(DirectX::XMVectorMultiply(e, a), DirectX::XMVectorMultiply(d, b)));
    float z = (x + y) - ac_bb;

    uint32_t i = (uint32_t&)x;
    uint32_t j = (uint32_t&)y;
    uint32_t k = (uint32_t&)z;

    return ((k & ~(i | j)) & 0x80000000) != 0;
}

namespace _OSM_INTERNAL_
{
    _Success_(return)
    inline bool OSMCALL GetLowestRoot(_In_ DirectX::FXMVECTOR a, _In_ DirectX::FXMVECTOR b, _In_ DirectX::FXMVECTOR c, _In_ DirectX::FXMVECTOR maxR, _Out_ DirectX::XMVECTOR& root)
    {
        const DirectX::XMVECTOR zero = DirectX::XMVectorZero();
        const DirectX::XMVECTOR one = DirectX::XMVectorSet(1, 1, 1, 1);
        const DirectX::XMVECTOR two = DirectX::XMVectorSet(2, 2, 2, 2);
        const DirectX::XMVECTOR four = DirectX::XMVectorSet(4, 4, 4, 4);

        DirectX::XMVECTOR det = DirectX::XMVectorSubtract(DirectX::XMVectorMultiply(b, b), DirectX::XMVectorMultiply(four, DirectX::XMVectorMultiply(a, c)));

        // If determinant is negative, there are no solutions
        if (DirectX::XMVector3Less(det, zero))
        {
            return false;
        }

        DirectX::XMVECTOR sqrtD = DirectX::XMVectorSqrt(det);
        DirectX::XMVECTOR inv2a = DirectX::XMVectorDivide(one, DirectX::XMVectorMultiply(two, a));
        DirectX::XMVECTOR negB = DirectX::XMVectorNegate(b);
        DirectX::XMVECTOR r1 = DirectX::XMVectorMultiply(DirectX::XMVectorSubtract(negB, sqrtD), inv2a);
        DirectX::XMVECTOR r2 = DirectX::XMVectorMultiply(DirectX::XMVectorAdd(negB, sqrtD), inv2a);

        // Sort so r1 <= r2
        if (DirectX::XMVector3Greater(r1, r2))
        {
            DirectX::XMVECTOR temp = r2;
            r2 = r1;
            r1 = temp;
        }

        // Get lowest root
        if (DirectX::XMVector3Greater(r1, zero) && DirectX::XMVector3Less(r1, maxR))
        {
            root = r1;
            return true;
        }

        // If r1 < 0, r2 might be okay
        if (DirectX::XMVector3Greater(r2, zero) && DirectX::XMVector3Less(r2, maxR))
        {
            root = r2;
            return true;
        }

        // No valid solutions
        return false;
    }

    inline void OSMCALL CheckVertex(_In_ const CollisionQuery& query, _In_ DirectX::FXMVECTOR v, _Inout_ DirectX::XMVECTOR& t, _Inout_ DirectX::XMVECTOR& collisionPoint, _Inout_ bool& collided)
    {
        const DirectX::XMVECTOR one = DirectX::XMVectorSet(1, 1, 1, 1);
        const DirectX::XMVECTOR two = DirectX::XMVectorSet(2, 2, 2, 2);

        DirectX::XMVECTOR a = query.VelocityLen2;
        DirectX::XMVECTOR b = DirectX::XMVectorMultiply(two, DirectX::XMVector3Dot(query.Velocity, DirectX::XMVectorSubtract(query.BasePoint, v)));
        DirectX::XMVECTOR c = DirectX::XMVectorSubtract(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(v, query.BasePoint)), one);

        DirectX::XMVECTOR newT;
        if (GetLowestRoot(a, b, c, t, newT))
        {
            t = newT;
            collisionPoint = v;
            collided = true;
        }
    }

    inline void OSMCALL CheckEdge(_In_ const CollisionQuery& query, _In_ DirectX::FXMVECTOR v0, _In_ DirectX::FXMVECTOR v1, _Inout_ DirectX::XMVECTOR& t, _Inout_ DirectX::XMVECTOR& collisionPoint, _Inout_ bool& collided)
    {
        const DirectX::XMVECTOR zero = DirectX::XMVectorZero();
        const DirectX::XMVECTOR one = DirectX::XMVectorSet(1, 1, 1, 1);
        const DirectX::XMVECTOR two = DirectX::XMVectorSet(2, 2, 2, 2);

        DirectX::XMVECTOR edge = DirectX::XMVectorSubtract(v1, v0);
        DirectX::XMVECTOR baseToVertex = DirectX::XMVectorSubtract(v0, query.BasePoint);
        DirectX::XMVECTOR edgeLen2 = DirectX::XMVector3LengthSq(edge);
        DirectX::XMVECTOR edgeDotV = DirectX::XMVector3Dot(edge, query.Velocity);
        DirectX::XMVECTOR edgeDotBaseToV = DirectX::XMVector3Dot(edge, baseToVertex);

        DirectX::XMVECTOR a = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(edgeLen2, DirectX::XMVectorNegate(query.VelocityLen2)), DirectX::XMVectorMultiply(edgeDotV, edgeDotV));
        DirectX::XMVECTOR b = DirectX::XMVectorSubtract(DirectX::XMVectorMultiply(edgeLen2, DirectX::XMVectorMultiply(two, DirectX::XMVector3Dot(query.Velocity, baseToVertex))),
            DirectX::XMVectorMultiply(DirectX::XMVectorMultiply(two, edgeDotV), edgeDotBaseToV));
        DirectX::XMVECTOR c = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(edgeLen2, DirectX::XMVectorSubtract(one, DirectX::XMVector3LengthSq(baseToVertex))),
            DirectX::XMVectorMultiply(edgeDotBaseToV, edgeDotBaseToV));

        // Collide with infinite edge?
        DirectX::XMVECTOR newT;
        if (GetLowestRoot(a, b, c, t, newT))
        {
            // Within line segment?
            DirectX::XMVECTOR f = DirectX::XMVectorDivide(DirectX::XMVectorSubtract(DirectX::XMVectorMultiply(edgeDotV, newT), edgeDotBaseToV), edgeLen2);
            if (DirectX::XMVector3GreaterOrEqual(f, zero) && DirectX::XMVector3LessOrEqual(f, one))
            {
                t = newT;
                collisionPoint = DirectX::XMVectorAdd(v0, DirectX::XMVectorMultiply(f, edge));
                collided = true;
            }
        }
    }

    inline bool OSMCALL PointInAABB(_In_ DirectX::FXMVECTOR p, _In_ const AABB& aabb)
    {
        return DirectX::XMVector2GreaterOrEqual(p, aabb.GetMinV()) && DirectX::XMVector2LessOrEqual(p, aabb.GetMaxV());
    }
} // _OSM_INTERNAL_

// Assumes that triangle vertices are in ellipsoid space
inline void OSMCALL TestEllipsoidTriangle(_Inout_ CollisionQuery& query, _In_ DirectX::FXMVECTOR v0, _In_ DirectX::XMVECTOR v1, _In_ DirectX::FXMVECTOR v2)
{
    const DirectX::XMVECTOR zero = DirectX::XMVectorZero();
    const DirectX::XMVECTOR one = DirectX::XMVectorSet(1, 1, 1, 1);

    // Compute the normal for the triangle's plane
    DirectX::XMVECTOR planeNormal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v1, v0), DirectX::XMVectorSubtract(v2, v0)));

    DirectX::XMVECTOR vDotN = DirectX::XMVector3Dot(query.Velocity, planeNormal);

    if (DirectX::XMVector3Greater(vDotN, zero))
    {
        // Skip backfacing
        return;
    }

    // Compute plane D value
    DirectX::XMVECTOR planeD = DirectX::XMVector3Dot(v0, planeNormal);

    DirectX::XMVECTOR t0, t1;
    bool embeddedInPlane = false;
    DirectX::XMVECTOR signedDistToPlane = DirectX::XMVectorSubtract(DirectX::XMVector3Dot(query.BasePoint, planeNormal), planeD);

    if (DirectX::XMVector3Equal(vDotN, zero))
    {
        // Moving parallel to the plane
        if (DirectX::XMVector3GreaterOrEqual(DirectX::XMVectorAbs(signedDistToPlane), one))
        {
            // Not embedded in the plane, so can't collide
            return;
        }
        else
        {
            // Embedded in plane, so intersects over the entire interval
            embeddedInPlane = true;
            t0 = zero;
            t1 = one;
        }
    }
    else
    {
        // Not parallel, compute intersection interval
        t0 = DirectX::XMVectorDivide(DirectX::XMVectorSubtract(one, signedDistToPlane), vDotN);
        t1 = DirectX::XMVectorDivide(DirectX::XMVectorSubtract(DirectX::XMVectorNegate(one), signedDistToPlane), vDotN);

        // Ensure t0 < t1 (swap if necessary)
        if (DirectX::XMVector3Greater(t0, t1))
        {
            DirectX::XMVECTOR temp = t0;
            t1 = t0;
            t0 = temp;
        }

        // Check that at least one of the results is within range
        if (DirectX::XMVector3Greater(t0, one) || DirectX::XMVector3Less(t1, zero))
        {
            // Both values are outside of [0, 1], so can't collide
            return;
        }

        // Clamp values to [0, 1]
        t0 = DirectX::XMVectorClamp(t0, zero, one);
        t1 = DirectX::XMVectorClamp(t1, zero, one);

        // We now have an interval [t0, t1] where the
        // object collides with the plane. Look for collision
        // with triangle.
        DirectX::XMVECTOR collisionPoint = zero;
        DirectX::XMVECTOR t = one;
        bool collided = false;

        // First, check for collision with face of triangle.
        // For this, the collision is at t0 and only if we're not
        // embedded.
        if (!embeddedInPlane)
        {
            DirectX::XMVECTOR planeIntersection = DirectX::XMVectorAdd(DirectX::XMVectorSubtract(query.BasePoint, planeNormal), DirectX::XMVectorMultiply(t0, query.Velocity));

            // Check if point is in triangle
            if (PointInTriangle(planeIntersection, v0, v1, v2))
            {
                collided = true;
                t = t0;
                collisionPoint = planeIntersection;
            }
        }

        // If we don't have a collision yet, then we need
        // to do swept test against vertices & edges
        if (!collided)
        {
            // Check vertices
            _OSM_INTERNAL_::CheckVertex(query, v0, t, collisionPoint, collided);
            _OSM_INTERNAL_::CheckVertex(query, v1, t, collisionPoint, collided);
            _OSM_INTERNAL_::CheckVertex(query, v2, t, collisionPoint, collided);

            // Check edges
            _OSM_INTERNAL_::CheckEdge(query, v0, v1, t, collisionPoint, collided);
            _OSM_INTERNAL_::CheckEdge(query, v1, v2, t, collisionPoint, collided);
            _OSM_INTERNAL_::CheckEdge(query, v2, v0, t, collisionPoint, collided);
        }

        if (collided)
        {
            DirectX::XMVECTOR dist = DirectX::XMVectorMultiply(t, DirectX::XMVectorSqrt(query.VelocityLen2));

            // is this closer than current best?
            if (!query.Collided || DirectX::XMVector3Less(dist, query.CollisionDistance))
            {
                query.CollisionDistance = dist;
                query.CollisionPoint = collisionPoint;
                query.Collided = true;
            }
        }
    }
}

// Assumes that aabb points are in ellipsoid space
inline void OSMCALL TestEllipsoidAABB(_Inout_ CollisionQuery& query, _In_ const AABB& aabb)
{
    // Moving ellipsoid can only be closing in on 3 of the 6 planes (the other 3 are back-facing)
    DirectX::XMFLOAT3 vel;
    DirectX::XMStoreFloat3(&vel, query.Velocity);

    DirectX::XMVECTOR center = aabb.GetCenterV();
    DirectX::XMFLOAT3 extents = aabb.GetExtents();

    DirectX::XMVECTOR verts[4];

    // X
    if (vel.x > 0.0f)
    {
        // test min x side
        verts[0] = DirectX::XMVectorSet(-extents.x, -extents.y, extents.z, 0);
        verts[1] = DirectX::XMVectorSet(-extents.x, extents.y, extents.z, 0);
        verts[2] = DirectX::XMVectorSet(-extents.x, extents.y, -extents.z, 0);
        verts[3] = DirectX::XMVectorSet(-extents.x, -extents.y, -extents.z, 0);
    }
    else
    {
        // test max x side
        verts[0] = DirectX::XMVectorSet(extents.x, -extents.y, -extents.z, 0);
        verts[1] = DirectX::XMVectorSet(extents.x, extents.y, -extents.z, 0);
        verts[2] = DirectX::XMVectorSet(extents.x, extents.y, extents.z, 0);
        verts[3] = DirectX::XMVectorSet(extents.x, -extents.y, extents.z, 0);
    }

    TestEllipsoidTriangle(query, DirectX::XMVectorAdd(center, verts[0]), DirectX::XMVectorAdd(center, verts[1]), DirectX::XMVectorAdd(center, verts[2]));
    TestEllipsoidTriangle(query, DirectX::XMVectorAdd(center, verts[0]), DirectX::XMVectorAdd(center, verts[2]), DirectX::XMVectorAdd(center, verts[3]));

    // Y
    if (vel.y > 0.0f)
    {
        // test min y side
        verts[0] = DirectX::XMVectorSet(-extents.x, -extents.y, extents.z, 0);
        verts[1] = DirectX::XMVectorSet(-extents.x, -extents.y, -extents.z, 0);
        verts[2] = DirectX::XMVectorSet(extents.x, -extents.y, -extents.z, 0);
        verts[3] = DirectX::XMVectorSet(extents.x, -extents.y, extents.z, 0);
    }
    else
    {
        // test max y side
        verts[0] = DirectX::XMVectorSet(-extents.x, extents.y, -extents.z, 0);
        verts[1] = DirectX::XMVectorSet(-extents.x, extents.y, extents.z, 0);
        verts[2] = DirectX::XMVectorSet(extents.x, extents.y, extents.z, 0);
        verts[3] = DirectX::XMVectorSet(extents.x, extents.y, -extents.z, 0);
    }

    TestEllipsoidTriangle(query, DirectX::XMVectorAdd(center, verts[0]), DirectX::XMVectorAdd(center, verts[1]), DirectX::XMVectorAdd(center, verts[2]));
    TestEllipsoidTriangle(query, DirectX::XMVectorAdd(center, verts[0]), DirectX::XMVectorAdd(center, verts[2]), DirectX::XMVectorAdd(center, verts[3]));

    // Z
    if (vel.z > 0.0f)
    {
        // test min z side
        verts[0] = DirectX::XMVectorSet(-extents.x, -extents.y, -extents.z, 0);
        verts[1] = DirectX::XMVectorSet(-extents.x, extents.y, -extents.z, 0);
        verts[2] = DirectX::XMVectorSet(extents.x, extents.y, -extents.z, 0);
        verts[3] = DirectX::XMVectorSet(extents.x, -extents.y, -extents.z, 0);
    }
    else
    {
        // test max z side
        verts[0] = DirectX::XMVectorSet(extents.x, -extents.y, extents.z, 0);
        verts[1] = DirectX::XMVectorSet(extents.x, extents.y, extents.z, 0);
        verts[2] = DirectX::XMVectorSet(-extents.x, extents.y, extents.z, 0);
        verts[3] = DirectX::XMVectorSet(-extents.x, -extents.y, extents.z, 0);
    }

    TestEllipsoidTriangle(query, DirectX::XMVectorAdd(center, verts[0]), DirectX::XMVectorAdd(center, verts[1]), DirectX::XMVectorAdd(center, verts[2]));
    TestEllipsoidTriangle(query, DirectX::XMVectorAdd(center, verts[0]), DirectX::XMVectorAdd(center, verts[2]), DirectX::XMVectorAdd(center, verts[3]));
}

// Function returns true if no resolve was necessary. Otherwise
// false to indicate further collision testing is required.
//
// NOTE: Only resolves the ellipsoid values. In other words, upon
// completion, BasePoint == final location, and Velocity == final movement.
// These can then be converted into new WorldPosition & WorldMovement
// values by the caller. Keeping them separate allows multiple collision/resolves
// to be chained together (ex: gravity)
inline bool OSMCALL ResolveCollision(_Inout_ CollisionQuery& query)
{
    const DirectX::XMVECTOR veryCloseDistance = DirectX::XMVectorSet(0.0001f, 0.0001f, 0.0001f, 0.0001f);

    DirectX::XMVECTOR destinationPoint = DirectX::XMVectorAdd(query.BasePoint, query.Velocity);

    if (!query.Collided)
    {
        query.BasePoint = destinationPoint;
        return true;
    }

    // Needs to be resolved
    // NOTE: These computations are all in ellipsoid space

    DirectX::XMVECTOR newBasePoint = query.BasePoint;

    // Only update if we're not already very close, and if so, then
    // we only move very close to intersection, not exact (helps with stability)
    if (DirectX::XMVector3GreaterOrEqual(query.CollisionDistance, veryCloseDistance))
    {
        DirectX::XMVECTOR v = query.Velocity;
        v = DirectX::XMVectorMultiply(DirectX::XMVector3Normalize(v), DirectX::XMVectorSubtract(query.CollisionDistance, veryCloseDistance));
        newBasePoint = DirectX::XMVectorAdd(query.BasePoint, v);

        // Adjust collision point
        query.CollisionPoint = DirectX::XMVectorSubtract(query.CollisionPoint, DirectX::XMVectorMultiply(veryCloseDistance, DirectX::XMVector3Normalize(v)));
    }

    // Determine the sliding plane
    DirectX::XMVECTOR planeNormal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(newBasePoint, query.CollisionPoint));
    DirectX::XMVECTOR planeD = DirectX::XMVector3Dot(planeNormal, query.CollisionPoint);

    DirectX::XMVECTOR signedDist = DirectX::XMVectorSubtract(DirectX::XMVector3Dot(planeNormal, destinationPoint), planeD);
    DirectX::XMVECTOR newDestinationPoint = DirectX::XMVectorSubtract(destinationPoint, DirectX::XMVectorMultiply(signedDist, planeNormal));

    // Generate the slide vector (new velocity vector)
    query.Velocity = DirectX::XMVectorSubtract(newDestinationPoint, query.CollisionPoint);
    query.VelocityLen2 = DirectX::XMVector3LengthSq(query.Velocity);
    query.BasePoint = newBasePoint;
    query.Collided = false;

    // If new velocity is small enough, then we're done
    return (DirectX::XMVector3Less(DirectX::XMVector3Length(query.Velocity), veryCloseDistance));
}
