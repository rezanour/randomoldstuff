#include "precomp.h"
#include "kdtree.h"

static const float epsilon = 0.0001f;

static inline float DistToPlane(_In_ const XMVECTOR& plane, _In_ const XMVECTOR& position)
{
    // dot replicates answer to all 4 components. Subtract plane means that the D value (in w) will be
    // subtracted from the w of the dot. Finally, we then extract the w.
    return XMVectorGetW(XMVectorSubtract(XMVector3Dot(plane, position), plane));
}

static inline float DistToPlane(_In_ const XMFLOAT4& plane, _In_ const XMFLOAT3& position)
{
    return plane.x * position.x + plane.y * position.y + plane.z * position.z - plane.w;
}

#define PUSH(pHead, t) t->Next = *pHead; *pHead = t;
#if 0
#define ABS(f) (float)((uint32_t)f & 0x7FFFFFFF)
#else
#define ABS fabsf
#endif

_Use_decl_annotations_
KdTreeCompiler* KdTreeCompiler::CreateFromTriangles(const StaticGeometryVertex* vertices, uint32_t numVertices, const uint32_t* indices, uint32_t numIndices)
{
    UNREFERENCED_PARAMETER(numVertices);

    KdTreeCompiler* compiler = new KdTreeCompiler();

    XMFLOAT3 boundsMin(FLT_MAX, FLT_MAX, FLT_MAX);
    XMFLOAT3 boundsMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    KdCompilerTriangle* triangles = nullptr;
    KdCompilerTriangle* tail = nullptr;
    for (uint32_t i = 0; i < numIndices / 3; ++i)
    {
        KdCompilerTriangle* t = new KdCompilerTriangle(vertices[indices[i * 3]], vertices[indices[i * 3 + 1]], vertices[indices[i * 3 + 2]]);
        if (triangles == nullptr)
        {
            triangles = t;
        }

        if (tail == nullptr)
        {
            tail = t;
        }
        else
        {
            tail->Next = t;
            tail = t;
        }

        boundsMin = VecMin(boundsMin, t->Min);
        boundsMax = VecMax(boundsMax, t->Max);
    }

    compiler->_normals[0] = XMFLOAT3(1, 0, 0);
    compiler->_normals[1] = XMFLOAT3(0, 1, 0);
    compiler->_normals[2] = XMFLOAT3(0, 0, 1);

    compiler->_root = compiler->Process(&triangles, AABB(boundsMin, boundsMax));

    return compiler;
}

KdTreeCompiler::KdTreeCompiler() :
    _root(nullptr)
{
}

KdTreeCompiler::~KdTreeCompiler()
{
    delete _root;
}

_Use_decl_annotations_
KdTreeCompiler::KdCompilerNode* KdTreeCompiler::Process(KdCompilerTriangle** triangles, const AABB& bounds)
{
    if (*triangles == nullptr)
    {
        Error("Invalid state");
    }

    KdCompilerNode* node = new KdCompilerNode();

    KdCompilerTriangle* frontHead = nullptr;
    KdCompilerTriangle* backHead = nullptr;
    AABB frontBounds, backBounds;
    uint32_t axis = 0;
    float value = 0.0f;

    if (SplitTriangles(triangles, bounds, &frontHead, &frontBounds, &backHead, &backBounds, &axis, &value))
    {
        node->Axis = axis;
        node->Value = value;
        node->Front = Process(&frontHead, frontBounds);
        node->Back = Process(&backHead, backBounds);
    }
    else
    {
        // too few to make a node, make it a sector
        node->Axis = 3;
        node->Head = *triangles;
    }

    return node;
}

_Use_decl_annotations_
bool KdTreeCompiler::SplitTriangles(KdCompilerTriangle** triangles, const AABB& bounds, KdCompilerTriangle** front, AABB* frontBounds, KdCompilerTriangle** back, AABB* backBounds, uint32_t* axis, float* value)
{
    // Find longest axis
    float diff[] =
    {
        bounds.GetMax().x - bounds.GetMin().x,
        bounds.GetMax().y - bounds.GetMin().y,
        bounds.GetMax().z - bounds.GetMin().z
    };

    uint32_t a = 0;
    for (uint32_t i = 1; i < 3; ++i)
    {
        if (diff[i] > diff[a])
        {
            a = i;
        }
    }

    // Find mean of the centroids along this axis
    float sum = 0.0f;
    uint32_t numTriangles = 0;
    KdCompilerTriangle* t = *triangles;
    while (t != nullptr)
    {
        sum += *(&t->Centroid.x + a);
        ++numTriangles;

        t = t->Next;
    }

    if (numTriangles < 10)
    {
        return false;
    }

    float mean = sum / (float)numTriangles;
    XMFLOAT4 plane(_normals[a].x, _normals[a].y, _normals[a].z, mean);

    XMFLOAT3 frontBoundsMin(FLT_MAX, FLT_MAX, FLT_MAX);
    XMFLOAT3 frontBoundsMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    XMFLOAT3 backBoundsMin(FLT_MAX, FLT_MAX, FLT_MAX);
    XMFLOAT3 backBoundsMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    // Split the triangles into each group
    t = *triangles;
    while (t != nullptr)
    {
        KdCompilerTriangle* next = t->Next;

        if (*(&t->Min.x + a) >= mean)
        {
            // KdCompilerTriangle is fully in front of plane
            PUSH(front, t);

            frontBoundsMin = VecMin(frontBoundsMin, t->Min);
            frontBoundsMax = VecMax(frontBoundsMax, t->Max);
        }
        else if (*(&t->Max.x + a) < mean)
        {
            // KdCompilerTriangle is fully behind plane
            PUSH(back, t);

            backBoundsMin = VecMin(backBoundsMin, t->Min);
            backBoundsMax = VecMax(backBoundsMax, t->Max);
        }
        else
        {
            SplitTriangle(t, plane, front, back);
        }

        t = next;
    }

    if (*back == nullptr)
    {
        delete *triangles;
        *triangles = *front;
        return false;
    }

    if (*front == nullptr)
    {
        delete *triangles;
        *triangles = *back;
        return false;
    }

    *axis = a;
    *value = mean;
    *frontBounds = AABB(frontBoundsMin, frontBoundsMax);
    *backBounds = AABB(backBoundsMin, backBoundsMax);

    return true;
}

_Use_decl_annotations_
void KdTreeCompiler::SplitTriangle(const KdCompilerTriangle* t, const XMFLOAT4& plane, KdCompilerTriangle** front, KdCompilerTriangle** back)
{
    // are any of the points on the plane
    float d0 = DistToPlane(plane, t->V0.Position);
    float d1 = DistToPlane(plane, t->V1.Position);
    float d2 = DistToPlane(plane, t->V2.Position);

    if (ABS(d0) < epsilon)
    {
        // V0 is on plane, split V1-V2 edge
        StaticGeometryVertex vNew;
        SplitEdge(t->V1, t->V2, plane, &vNew);

        KdCompilerTriangle* t1 = new KdCompilerTriangle(t->V0, t->V1, vNew);
        KdCompilerTriangle* t2 = new KdCompilerTriangle(t->V0, vNew, t->V2);

        if (d1 > 0)
        {
            PUSH(front, t1);
            PUSH(back, t2);
        }
        else
        {
            PUSH(back, t1);
            PUSH(front, t2);
        }
    }
    else if (ABS(d1) < epsilon)
    {
        // V1 is on plane
        StaticGeometryVertex vNew;
        SplitEdge(t->V0, t->V2, plane, &vNew);

        KdCompilerTriangle* t1 = new KdCompilerTriangle(t->V0, t->V1, vNew);
        KdCompilerTriangle* t2 = new KdCompilerTriangle(t->V1, t->V2, vNew);

        if (d0 > 0)
        {
            PUSH(front, t1);
            PUSH(back, t2);
        }
        else
        {
            PUSH(back, t1);
            PUSH(front, t2);
        }
    }
    else if (ABS(d2) < epsilon)
    {
        // V2 is on plane
        StaticGeometryVertex vNew;
        SplitEdge(t->V0, t->V1, plane, &vNew);

        KdCompilerTriangle* t1 = new KdCompilerTriangle(t->V0, vNew, t->V2);
        KdCompilerTriangle* t2 = new KdCompilerTriangle(vNew, t->V1, t->V2);

        if (d0 > 0)
        {
            PUSH(front, t1);
            PUSH(back, t2);
        }
        else
        {
            PUSH(back, t1);
            PUSH(front, t2);
        }
    }
    else
    {
        // none are on the plane, do a 1-2 split
        if (d0 > -epsilon) // V0 is in front of plane
        {
            if (d1 > -epsilon) // V1 is in front of plane
            {
                // V2 is by itself
                StaticGeometryVertex vNew1, vNew2;
                SplitEdge(t->V2, t->V0, plane, &vNew1);
                SplitEdge(t->V2, t->V1, plane, &vNew2);

                KdCompilerTriangle* t1 = new KdCompilerTriangle(vNew1, vNew2, t->V2);
                KdCompilerTriangle* t2 = new KdCompilerTriangle(t->V0, t->V1, vNew2);
                KdCompilerTriangle* t3 = new KdCompilerTriangle(t->V0, vNew2, vNew1);

                PUSH(front, t2);
                PUSH(front, t3);
                PUSH(back, t1);
            }
            else // V1 is behind plane
            {
                if (d2 > -epsilon)  // V2 is in front
                {
                    // V1 is by itself
                    StaticGeometryVertex vNew1, vNew2;
                    SplitEdge(t->V1, t->V2, plane, &vNew1);
                    SplitEdge(t->V1, t->V0, plane, &vNew2);

                    KdCompilerTriangle* t1 = new KdCompilerTriangle(vNew1, vNew2, t->V1);
                    KdCompilerTriangle* t2 = new KdCompilerTriangle(t->V2, t->V0, vNew2);
                    KdCompilerTriangle* t3 = new KdCompilerTriangle(t->V2, vNew2, vNew1);

                    PUSH(front, t2);
                    PUSH(front, t3);
                    PUSH(back, t1);
                }
                else // V2 is behind
                {
                    // V0 is by itself
                    StaticGeometryVertex vNew1, vNew2;
                    SplitEdge(t->V0, t->V1, plane, &vNew1);
                    SplitEdge(t->V0, t->V2, plane, &vNew2);

                    KdCompilerTriangle* t1 = new KdCompilerTriangle(vNew1, vNew2, t->V0);
                    KdCompilerTriangle* t2 = new KdCompilerTriangle(t->V1, t->V2, vNew2);
                    KdCompilerTriangle* t3 = new KdCompilerTriangle(t->V1, vNew2, vNew1);

                    PUSH(back, t2);
                    PUSH(back, t3);
                    PUSH(front, t1);
                }
            }
        }
        else // V0 is behind plane
        {
            if (d1 < epsilon)  // V1 is behind
            {
                // V2 is by itself
                StaticGeometryVertex vNew1, vNew2;
                SplitEdge(t->V2, t->V0, plane, &vNew1);
                SplitEdge(t->V2, t->V1, plane, &vNew2);

                KdCompilerTriangle* t1 = new KdCompilerTriangle(vNew1, vNew2, t->V2);
                KdCompilerTriangle* t2 = new KdCompilerTriangle(t->V0, t->V1, vNew2);
                KdCompilerTriangle* t3 = new KdCompilerTriangle(t->V0, vNew2, vNew1);

                PUSH(back, t2);
                PUSH(back, t3);
                PUSH(front, t1);
            }
            else // V1 is in front
            {
                if (d2 < epsilon) // V2 is behind
                {
                    // V1 is by itself
                    StaticGeometryVertex vNew1, vNew2;
                    SplitEdge(t->V1, t->V2, plane, &vNew1);
                    SplitEdge(t->V1, t->V0, plane, &vNew2);

                    KdCompilerTriangle* t1 = new KdCompilerTriangle(vNew1, vNew2, t->V1);
                    KdCompilerTriangle* t2 = new KdCompilerTriangle(t->V2, t->V0, vNew2);
                    KdCompilerTriangle* t3 = new KdCompilerTriangle(t->V2, vNew2, vNew1);

                    PUSH(back, t2);
                    PUSH(back, t3);
                    PUSH(front, t1);
                }
                else
                {
                    // V0 is by itself
                    StaticGeometryVertex vNew1, vNew2;
                    SplitEdge(t->V0, t->V1, plane, &vNew1);
                    SplitEdge(t->V0, t->V2, plane, &vNew2);

                    KdCompilerTriangle* t1 = new KdCompilerTriangle(vNew1, vNew2, t->V0);
                    KdCompilerTriangle* t2 = new KdCompilerTriangle(t->V1, t->V2, vNew2);
                    KdCompilerTriangle* t3 = new KdCompilerTriangle(t->V1, vNew2, vNew1);

                    PUSH(front, t2);
                    PUSH(front, t3);
                    PUSH(back, t1);
                }
            }
        }
    }
}

_Use_decl_annotations_
void KdTreeCompiler::SplitEdge(const StaticGeometryVertex& V0, const StaticGeometryVertex& V1, const XMFLOAT4& plane, StaticGeometryVertex* v)
{
    XMFLOAT3 vv0 = V0.Position;
    XMFLOAT3 vv1 = V1.Position;

    float d0 = DistToPlane(plane, vv0);

    XMVECTOR sub = XMVectorSubtract(XMLoadFloat3(&vv1), XMLoadFloat3(&vv0));
    XMVECTOR dir = XMVector3Normalize(sub);
    XMVECTOR n = XMLoadFloat4(&plane);
    float d = d0;
    if (d >= 0)
    {
        n = XMVectorNegate(n);
    }
    else if (d < 0)
    {
        d = -d;
    }

    float x = 0.5f;
    float denom = XMVectorGetX(XMVector3Dot(n, dir));
    if (ABS(denom) > epsilon)
    {
        x = d / denom;
    }
    XMStoreFloat3(&v->Position, XMVectorAdd(XMLoadFloat3(&vv0), XMVectorScale(dir, x)));
    assert(!isnan(v->Position.x));
    assert(!isnan(v->Position.y));
    assert(!isnan(v->Position.z));
}
