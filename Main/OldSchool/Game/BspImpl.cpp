#include "Precomp.h"
#include "Geometry.h"
#include "Bsp.h"
#include "BspImpl.h"

_Use_decl_annotations_
std::shared_ptr<Bsp> Bsp::Create(BspData& bsp)
{
    return std::shared_ptr<Bsp>(new Bsp(bsp));
}

_Use_decl_annotations_
Bsp::Bsp(BspData& bsp)
{
    _data.Root = bsp.Root;
    _data.Nodes.swap(bsp.Nodes);
    _data.Sectors.swap(bsp.Sectors);
    _data.Triangles.swap(bsp.Triangles);
}

const BspData& Bsp::GetData() const
{
    return _data;
}

_Use_decl_annotations_
bool Bsp::ClipLineSegment(const XMVECTOR& start, const XMVECTOR& end, ClipResult& result) const
{
    return ClipLineSegment(start, end, _data.Root, result);
}

_Use_decl_annotations_
void Bsp::CheckEllipsoid(CollisionQuery& query) const
{
    // TODO: Move this
    // Ensure World values are up to date
    query.WorldPosition = query.BasePoint * query.eRadius;
    query.WorldMovement = query.Velocity * query.eRadius;

    CheckEllipsoid(query, _data.Root);
}

_Use_decl_annotations_
bool Bsp::ClipLineSegment(const XMVECTOR& start, const XMVECTOR& end, int32_t nodeIndex, ClipResult& result) const
{
    if (nodeIndex >= 0)
    {
        // inner node
        const BspNode& node = _data.Nodes[nodeIndex];

        XMVECTOR plane = XMLoadFloat4(&node.Plane);
        XMVECTOR startD = XMVector3Dot(plane, start) - XMVectorSplatW(plane);

        if (XMVector3Greater(startD, XMVectorZero()))
        {
            // Start is in the front half space. Test front first
            if (ClipLineSegment(start, end, node.Front, result))
            {
                return true;
            }
        }
        else
        {
            // Start is on the negative half space
            if (ClipLineSegment(start, end, node.Back, result))
            {
                return true;
            }
        }

        // If we got this far, the segment wasn't clipped on the side tested.
        // IFF it crosses to the other side of the splitting plane, test the other
        // side too
        XMVECTOR endD = XMVector3Dot(plane, end) - XMVectorSplatW(plane);
        if (XMVector3Less(XMVectorMultiply(startD, endD), XMVectorZero()))
        {
            // Crosses plane, test other side
            if (XMVector3Greater(startD, XMVectorZero()))
            {
                return ClipLineSegment(start, end, node.Back, result);
            }
            else
            {
                return ClipLineSegment(start, end, node.Front, result);
            }
        }
    }
    else
    {
        // sector. Clip against the triangles
        const BspSector& sector = _data.Sectors[-(nodeIndex + 1)];
        bool haveResult = false;
        ClipResult tempResult;
        const BspTriangle* triangle = &_data.Triangles[sector.FirstTriangle];
        for (uint32_t i = 0; i < sector.NumTriangles; ++i, ++triangle)
        {
            if (TestLineSegmentTriangle(start, end, *triangle, tempResult))
            {
                if (!haveResult || tempResult.Dist < result.Dist)
                {
                    result = tempResult;
                    haveResult = true;
                }
            }
        }

        return haveResult;
    }

    return false;
}

_Use_decl_annotations_
bool Bsp::CheckEllipsoid(CollisionQuery& query, int32_t nodeIndex) const
{
    if (nodeIndex >= 0)
    {
        // inner node
        const BspNode& node = _data.Nodes[nodeIndex];

        XMVECTOR plane = XMLoadFloat4(&node.Plane);
        XMVECTOR offset = XMVectorAbs(XMVector3Dot(plane, query.eRadius));

        XMVECTOR start = query.WorldPosition;
        XMVECTOR end = query.WorldPosition + query.WorldMovement;

        XMVECTOR startD = XMVector3Dot(plane, start) - XMVectorSplatW(plane) - offset;
        if (XMVector3Greater(startD, XMVectorZero()))
        {
            // Start is in the front half space. Test front first
            if (CheckEllipsoid(query, node.Front))
            {
                return true;
            }
        }
        else
        {
            // Start is on the negative half space
            if (CheckEllipsoid(query, node.Back))
            {
                return true;
            }
        }

        // If we got this far, the segment wasn't clipped on the side tested.
        // IFF it crosses to the other side of the splitting plane, test the other
        // side too
        XMVECTOR endD = XMVector3Dot(plane, end) - XMVectorSplatW(plane) + offset;
        if (XMVector3Less(XMVectorMultiply(startD, endD), XMVectorZero()))
        {
            // Crosses plane, test other side
            if (XMVector3Greater(startD, XMVectorZero()))
            {
                return CheckEllipsoid(query, node.Back);
            }
            else
            {
                return CheckEllipsoid(query, node.Front);
            }
        }
    }
    else
    {
        // sector. Clip against the triangles
        const BspSector& sector = _data.Sectors[-(nodeIndex + 1)];
        const BspTriangle* triangle = &_data.Triangles[sector.FirstTriangle];
        bool clippedByThisSector = false;
        XMVECTOR currentNearest = query.Collided ? query.CollisionDistance : XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
        for (uint32_t i = 0; i < sector.NumTriangles; ++i, ++triangle)
        {
            TestEllipsoidTriangle(query, XMLoadFloat3(&triangle->V0.Position) / query.eRadius, XMLoadFloat3(&triangle->V1.Position) / query.eRadius, XMLoadFloat3(&triangle->V2.Position) / query.eRadius);
            if (!clippedByThisSector && query.Collided && XMVector3Less(query.CollisionDistance, currentNearest))
            {
                clippedByThisSector = true;
            }
        }

        return clippedByThisSector;
    }

    return false;
}

_Use_decl_annotations_
bool Bsp::TestLineSegmentTriangle(const XMVECTOR& start, const XMVECTOR& end, const BspTriangle& triangle, ClipResult& result)
{
    // Determine where the line segment crosses the triangle's plane (if at all)
    XMVECTOR plane = XMLoadFloat4(&triangle.Plane);
    XMVECTOR planeD = XMVectorSplatW(plane);

    XMVECTOR dir = end - start;
    XMVECTOR dirDotN = XMVector3Dot(dir, plane);

    if (XMVector3Equal(dirDotN, XMVectorZero()))
    {
        // parallel to plane. Is it embedded?
        if (XMVector3Equal(XMVector3Dot(start, plane), planeD))
        {
            // Hit, embedded in plane
            result.Dist = 0.0f;
            result.Plane = triangle.Plane;
            return true;
        }

        // Parallel, no hit
        return false;
    }
    else if (XMVector3Greater(dirDotN, XMVectorZero()))
    {
        // Ignore back-face
        return false;
    }

    XMVECTOR startD = XMVector3Dot(plane, start) - planeD;
    XMVECTOR endD = XMVector3Dot(plane, end) - planeD;

    // Can only cross if startD & endD are on opposite sides
    if (XMVector3Less(startD * endD, XMVectorZero()))
    {
        XMVECTOR t = XMVectorDivide(XMVectorAbs(startD), XMVectorAbs(startD) + XMVectorAbs(endD));
        XMVECTOR p = XMVectorMultiplyAdd(t, dir, start); // start + (t*dir)

        if (PointInTriangle(p, XMLoadFloat3(&triangle.V0.Position), XMLoadFloat3(&triangle.V1.Position), XMLoadFloat3(&triangle.V2.Position)))
        {
            XMStoreFloat(&result.Dist, XMVector3Length(t * dir));
            result.Plane = triangle.Plane;
            return true;
        }
    }

    return false;
}


//
// Algorithm for generating Bsp
//

static const float epsilon = 0.0001f;

static float DistToPlane(_In_ const XMVECTOR& plane, _In_ const XMVECTOR& position)
{
    // dot replicates answer to all 4 components. Subtract plane means that the D value (in w) will be
    // subtracted from the w of the dot. Finally, we then extract the w.
    return XMVectorGetW(XMVectorSubtract(XMVector3Dot(plane, position), plane));
}

static int32_t ProcessTriangles(_Inout_ BspData& bsp, _In_ const std::vector<BspTriangle>& triangles);

// Returns false if already convex (won't split then)
static bool FindBestSplit(_In_ const std::vector<BspTriangle>& triangles, _Out_ XMFLOAT4* splitPlane, _Out_ std::vector<BspTriangle>* front, _Out_ std::vector<BspTriangle>* back);

static void CreateSectorFromTriangles(_Inout_ BspData& bsp, _In_ const std::vector<BspTriangle>& triangles, _Out_ BspSector* sector);
static void SplitTriangle(_In_ const BspTriangle& t, _In_ const XMVECTOR& plane, _Inout_ std::vector<BspTriangle>* front, _Inout_ std::vector<BspTriangle>* back);
static void SplitEdge(_In_ const BspVertex& v0, _In_ const BspVertex& v1, _In_ const XMVECTOR& plane, _Out_ BspVertex* v);

_Use_decl_annotations_
std::shared_ptr<IBsp> GenerateBspFromTriangles(std::vector<BspTriangle>& triangles)
{
    BspData data;
    data.Root = ProcessTriangles(data, triangles);
    return Bsp::Create(data);
}

_Use_decl_annotations_
int32_t ProcessTriangles(BspData& bsp, const std::vector<BspTriangle>& triangles)
{
    // TODO: This is absolutely atrocious. Not only does this allocate like crazy, it also causes very large stack frames,
    // which make it easy to stack overflow even with relatively small levels.
    // This NEEDS to be fixed as soon as I get a chance to spend some time on it.
    std::vector<BspTriangle> front, back;

    XMFLOAT4 splitPlane;
    if (FindBestSplit(triangles, &splitPlane, &front, &back))
    {
        // Able to split the set of triangles. We need a node to desribe this
        BspNode node;
        node.Plane = splitPlane;
        node.Front = ProcessTriangles(bsp, front);
        node.Back = ProcessTriangles(bsp, back);
        bsp.Nodes.push_back(node);
        return static_cast<int32_t>(bsp.Nodes.size() - 1);
    }
    else
    {
        // Not able to split, already convex. Make it a sector
        BspSector sector;
        CreateSectorFromTriangles(bsp, triangles, &sector);
        bsp.Sectors.push_back(sector);
        return static_cast<int32_t>(bsp.Sectors.size() - 1) * -1 - 1;
    }
}

_Use_decl_annotations_
static bool FindBestSplit(const std::vector<BspTriangle>& triangles, XMFLOAT4* splitPlane, std::vector<BspTriangle>* front, std::vector<BspTriangle>* back)
{
    assert(triangles.size());

    front->clear();
    back->clear();

    // Find the best candidate for splitting on
    uint32_t bestCandidateIndex = static_cast<uint32_t>(triangles.size());
    uint32_t bestCandidateScore = static_cast<uint32_t>(triangles.size());

    const BspTriangle* triangle = triangles.data();
    for (uint32_t i = 0; i < triangles.size(); ++i, ++triangle)
    {
        XMVECTOR plane = XMLoadFloat4(&triangle->Plane);

        // Compute score
        int32_t score = 0;

        const BspTriangle* t = triangles.data();
        for (uint32_t j = 0; j < triangles.size(); ++j, ++t)
        {
            if (i == j)
            {
                // goes in front
                ++score;
                continue;
            }

            XMVECTOR tv0 = XMLoadFloat3(&t->V0.Position);
            XMVECTOR tv1 = XMLoadFloat3(&t->V1.Position);
            XMVECTOR tv2 = XMLoadFloat3(&t->V2.Position);
            XMVECTOR tplane = XMLoadFloat4(&t->Plane);

            float d0 = DistToPlane(plane, tv0);
            float d1 = DistToPlane(plane, tv1);
            float d2 = DistToPlane(plane, tv2);

            if (fabsf(d0) < epsilon && fabsf(d1) < epsilon && fabsf(d2) < epsilon)
            {
                // coplanar case
                if (XMVectorGetX(XMVector3Dot(plane, tplane)) > 0)
                {
                    // same facing direction
                    ++score;
                }
                else
                {
                    --score;
                }
            }
            else if (d0 > -epsilon && d1 > -epsilon && d2 > -epsilon)
            {
                // front of plane
                ++score;
            }
            else if (d0 < epsilon && d1 < epsilon && d2 < epsilon)
            {
                // fully behind plane
                --score;
            }
            else
            {
                // intersect, goes to both, so cancel each other out
            }
        }

        score = abs(score);

        if (static_cast<uint32_t>(score) < bestCandidateScore)
        {
            bestCandidateIndex = i;
            bestCandidateScore = score;
        }
    }

    // If didn't find candidate, and we're convex, then return false
    if (bestCandidateIndex == triangles.size())
    {
        *splitPlane = XMFLOAT4();
        return false;
    }

    const BspTriangle& split = triangles[bestCandidateIndex];

    XMVECTOR plane = XMLoadFloat4(&split.Plane);
    XMStoreFloat4(splitPlane, plane);

    const BspTriangle* t = triangles.data();
    for (uint32_t i = 0; i < triangles.size(); ++i, ++t)
    {
        if (bestCandidateIndex == i)
        {
            // self. insert in front
            front->push_back(*t);
            continue;
        }

        XMVECTOR tv0 = XMLoadFloat3(&t->V0.Position);
        XMVECTOR tv1 = XMLoadFloat3(&t->V1.Position);
        XMVECTOR tv2 = XMLoadFloat3(&t->V2.Position);
        XMVECTOR tplane = XMLoadFloat4(&t->Plane);

        float d0 = DistToPlane(plane, tv0);
        float d1 = DistToPlane(plane, tv1);
        float d2 = DistToPlane(plane, tv2);

        if (fabsf(d0) < epsilon && fabsf(d1) < epsilon && fabsf(d2) < epsilon)
        {
            // coplanar case
            if (XMVectorGetX(XMVector3Dot(plane, tplane)) > 0)
            {
                // same facing direction
                front->push_back(*t);
            }
            else
            {
                back->push_back(*t);
            }
        }
        else if (d0 > -epsilon && d1 > -epsilon && d2 > -epsilon)
        {
            // front of plane
            front->push_back(*t);
        }
        else if (d0 < epsilon && d1 < epsilon && d2 < epsilon)
        {
            // fully behind plane
            back->push_back(*t);
        }
        else
        {
            // intersect
            SplitTriangle(*t, plane, front, back);
        }
    }

    return true;
}

_Use_decl_annotations_
void CreateSectorFromTriangles(BspData& bsp, const std::vector<BspTriangle>& triangles, BspSector* sector)
{
    sector->FirstTriangle = static_cast<uint32_t>(bsp.Triangles.size());
    sector->NumTriangles = static_cast<uint32_t>(triangles.size());

    for (auto triangle : triangles)
    {
        bsp.Triangles.push_back(triangle);
    }
}

_Use_decl_annotations_
void SplitTriangle(const BspTriangle& t, const XMVECTOR& plane, std::vector<BspTriangle>* front, std::vector<BspTriangle>* back)
{
    // are any of the points on the plane
    XMVECTOR v0 = XMLoadFloat3(&t.V0.Position);
    XMVECTOR v1 = XMLoadFloat3(&t.V1.Position);
    XMVECTOR v2 = XMLoadFloat3(&t.V2.Position);

    float d0 = DistToPlane(plane, v0);
    float d1 = DistToPlane(plane, v1);
    float d2 = DistToPlane(plane, v2);

    if (fabsf(d0) < epsilon)
    {
        // v0 is on plane, split v1-v2 edge
        BspVertex vNew;
        SplitEdge(t.V1, t.V2, plane, &vNew);

        BspTriangle t1(t.V0.Position, t.V1.Position, vNew.Position);
        BspTriangle t2(t.V0.Position, vNew.Position, t.V2.Position);

        if (d1 > 0)
        {
            front->push_back(t1);
            back->push_back(t2);
        }
        else
        {
            back->push_back(t1);
            front->push_back(t2);
        }
    }
    else if (fabsf(d1) < epsilon)
    {
        // v1 is on plane
        BspVertex vNew;
        SplitEdge(t.V0, t.V2, plane, &vNew);

        BspTriangle t1(t.V0.Position, t.V1.Position, vNew.Position);
        BspTriangle t2(t.V1.Position, t.V2.Position, vNew.Position);

        if (d0 > 0)
        {
            front->push_back(t1);
            back->push_back(t2);
        }
        else
        {
            back->push_back(t1);
            front->push_back(t2);
        }
    }
    else if (fabsf(d2) < epsilon)
    {
        // v2 is on plane
        BspVertex vNew;
        SplitEdge(t.V0, t.V1, plane, &vNew);

        BspTriangle t1(t.V0.Position, vNew.Position, t.V2.Position);
        BspTriangle t2(vNew.Position, t.V1.Position, t.V2.Position);

        if (d0 > 0)
        {
            front->push_back(t1);
            back->push_back(t2);
        }
        else
        {
            back->push_back(t1);
            front->push_back(t2);
        }
    }
    else
    {
        // none are on the plane, do a 1-2 split
        if (d0 > -epsilon) // v0 is in front of plane
        {
            if (d1 > -epsilon) // v1 is in front of plane
            {
                // v2 is by itself
                BspVertex vNew1, vNew2;
                SplitEdge(t.V2, t.V0, plane, &vNew1);
                SplitEdge(t.V2, t.V1, plane, &vNew2);

                BspTriangle t1(vNew1.Position, vNew2.Position, t.V2.Position);
                BspTriangle t2(t.V0.Position, t.V1.Position, vNew2.Position);
                BspTriangle t3(t.V0.Position, vNew2.Position, vNew1.Position);

                front->push_back(t2);
                front->push_back(t3);
                back->push_back(t1);
            }
            else // v1 is behind plane
            {
                if (d2 > -epsilon)  // v2 is in front
                {
                    // v1 is by itself
                    BspVertex vNew1, vNew2;
                    SplitEdge(t.V1, t.V2, plane, &vNew1);
                    SplitEdge(t.V1, t.V0, plane, &vNew2);

                    BspTriangle t1(vNew1.Position, vNew2.Position, t.V1.Position);
                    BspTriangle t2(t.V2.Position, t.V0.Position, vNew2.Position);
                    BspTriangle t3(t.V2.Position, vNew2.Position, vNew1.Position);

                    front->push_back(t2);
                    front->push_back(t3);
                    back->push_back(t1);
                }
                else // v2 is behind
                {
                    // v0 is by itself
                    BspVertex vNew1, vNew2;
                    SplitEdge(t.V0, t.V1, plane, &vNew1);
                    SplitEdge(t.V0, t.V2, plane, &vNew2);

                    BspTriangle t1(vNew1.Position, vNew2.Position, t.V0.Position);
                    BspTriangle t2(t.V1.Position, t.V2.Position, vNew2.Position);
                    BspTriangle t3(t.V1.Position, vNew2.Position, vNew1.Position);

                    back->push_back(t2);
                    back->push_back(t3);
                    front->push_back(t1);
                }
            }
        }
        else // v0 is behind plane
        {
            if (d1 < epsilon)  // v1 is behind
            {
                // v2 is by itself
                BspVertex vNew1, vNew2;
                SplitEdge(t.V2, t.V0, plane, &vNew1);
                SplitEdge(t.V2, t.V1, plane, &vNew2);

                BspTriangle t1(vNew1.Position, vNew2.Position, t.V2.Position);
                BspTriangle t2(t.V0.Position, t.V1.Position, vNew2.Position);
                BspTriangle t3(t.V0.Position, vNew2.Position, vNew1.Position);

                back->push_back(t2);
                back->push_back(t3);
                front->push_back(t1);
            }
            else // v1 is in front
            {
                if (d2 < epsilon) // v2 is behind
                {
                    // v1 is by itself
                    BspVertex vNew1, vNew2;
                    SplitEdge(t.V1, t.V2, plane, &vNew1);
                    SplitEdge(t.V1, t.V0, plane, &vNew2);

                    BspTriangle t1(vNew1.Position, vNew2.Position, t.V1.Position);
                    BspTriangle t2(t.V2.Position, t.V0.Position, vNew2.Position);
                    BspTriangle t3(t.V2.Position, vNew2.Position, vNew1.Position);

                    back->push_back(t2);
                    back->push_back(t3);
                    front->push_back(t1);
                }
                else
                {
                    // v0 is by itself
                    BspVertex vNew1, vNew2;
                    SplitEdge(t.V0, t.V1, plane, &vNew1);
                    SplitEdge(t.V0, t.V2, plane, &vNew2);

                    BspTriangle t1(vNew1.Position, vNew2.Position, t.V0.Position);
                    BspTriangle t2(t.V1.Position, t.V2.Position, vNew2.Position);
                    BspTriangle t3(t.V1.Position, vNew2.Position, vNew1.Position);

                    front->push_back(t2);
                    front->push_back(t3);
                    back->push_back(t1);
                }
            }
        }
    }
}

_Use_decl_annotations_
void SplitEdge(const BspVertex& v0, const BspVertex& v1, const XMVECTOR& plane, BspVertex* v)
{
    XMVECTOR vv0 = XMLoadFloat3(&v0.Position);
    XMVECTOR vv1 = XMLoadFloat3(&v1.Position);

    float d0 = DistToPlane(plane, vv0);

    XMVECTOR sub = XMVectorSubtract(vv1, vv0);
    XMVECTOR dir = XMVector3Normalize(sub);
    XMVECTOR n = plane;
    float d = d0;
    if (d > 0)
    {
        n = XMVectorNegate(n);
    }
    else if (d < 0)
    {
        d = -d;
    }
    else
    {
        assert(false);
    }

    float x = d / XMVectorGetX(XMVector3Dot(n, dir));
    XMStoreFloat3(&v->Position, XMVectorAdd(vv0, XMVectorScale(dir, x)));
}
