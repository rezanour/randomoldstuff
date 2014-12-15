#include "precomp.h"
#include "kdtree2.h"
#include <DirectXCollision.h>

_Use_decl_annotations_
KdTreeCompiler2* KdTreeCompiler2::CreateFromTriangles(const StaticGeometryVertex* vertices, uint32_t numVertices, const uint32_t* indices, uint32_t numIndices)
{
    UNREFERENCED_PARAMETER(vertices);
    UNREFERENCED_PARAMETER(numVertices);
    UNREFERENCED_PARAMETER(indices);
    UNREFERENCED_PARAMETER(numIndices);

    std::unique_ptr<KdTreeCompiler2> kdTree(new KdTreeCompiler2());

    uint32_t numTriangles = numIndices / 3;

    kdTree->_vertices.resize(numVertices);
    for (uint32_t i = 0; i < numVertices; ++i)
    {
        kdTree->_vertices[i] = vertices[i];
    }

    float min[3], max[3];
    uint32_t tri = kdTree->AllocCompilerTriangle(vertices, indices[0], indices[1], indices[2]);
    memcpy_s(min, sizeof(min), kdTree->_compilerTriangles[0].Min, sizeof(min));
    memcpy_s(max, sizeof(max), kdTree->_compilerTriangles[0].Max, sizeof(max));
    for (uint32_t i = 1; i < numTriangles; ++i)
    {
        uint32_t triNext = kdTree->AllocCompilerTriangle(vertices, indices[i * 3], indices[i * 3 + 1], indices[i * 3 + 2]);

        KdCompilerTriangle* t = &kdTree->_compilerTriangles[triNext];
        for (uint32_t axis = 0; axis < 3; ++axis)
        {
            if (t->Min[axis] < min[axis]) min[axis] = t->Min[axis];
            if (t->Max[axis] > max[axis]) max[axis] = t->Max[axis];
        }

        kdTree->_compilerTriangles[tri].Next = triNext;
        tri = triNext;
    }

    kdTree->_root = kdTree->AllocNode();
    kdTree->Process(0, 0, numTriangles, min, max);

    return kdTree.release();
}

KdTreeCompiler2::KdTreeCompiler2() :
    _root(0)
{
}

_Use_decl_annotations_
void KdTreeCompiler2::BuildVisibleIndexList(const XMMATRIX& cameraWorld, const XMMATRIX& projection, uint32_t* indices, uint32_t maxIndices, uint32_t* numIndices, uint32_t* materials, uint32_t maxMaterials, uint32_t* numMaterials)
{
    *numIndices = 0;
    *numMaterials = 0;

    // TODO: Remove dependency on BoundingFrustum. Since all we
    // need is the 4 corners, we can derive those directly from view & projection
    BoundingFrustum frustum;
    BoundingFrustum::CreateFromMatrix(frustum, projection);
    frustum.Transform(frustum, cameraWorld);

    XMFLOAT3 corners[8];
    frustum.GetCorners(corners);

    XMFLOAT3 min = corners[0];
    XMFLOAT3 max = corners[0];

    for (uint32_t i = 1; i < 8; ++i)
    {
        min = VecMin(min, corners[i]);
        max = VecMax(max, corners[i]);
    }

    XMFLOAT3 position;
    XMStoreFloat3(&position, cameraWorld.r[3]);
    AppendVisibleIndices(&position.x, &min.x, &max.x, _root, indices, maxIndices, numIndices, materials, maxMaterials, numMaterials);
}

//
// Private
//

uint32_t KdTreeCompiler2::AllocNode()
{
    uint32_t i = (uint32_t)_nodes.size();
    _nodes.resize(i + 1);
    return i;
}

uint32_t KdTreeCompiler2::AllocTriangle(const KdCompilerTriangle& triangle)
{
    uint32_t i = (uint32_t)_triangles.size();
    _triangles.emplace_back(KdTriangle(triangle.Indices[0], triangle.Indices[1], triangle.Indices[2], triangle.MaterialId));
    return i;
}

uint32_t KdTreeCompiler2::AllocCompilerTriangle(const StaticGeometryVertex* vertices, uint32_t i0, uint32_t i1, uint32_t i2)
{
    uint32_t i = (uint32_t)_compilerTriangles.size();
    _compilerTriangles.emplace_back(KdCompilerTriangle(vertices, i0, i1, i2));
    return i;
}

_Use_decl_annotations_
void KdTreeCompiler2::Process(uint32_t nodeIndex, uint32_t trianglesRoot, uint32_t numTriangles, const float min[3], const float max[3])
{
    // Find the best split. If this function fails, it means we can't/shouldn't split further,
    // and should just make this node into a leaf.
    uint32_t splitAxis, frontRoot, backRoot, frontCount, backCount;
    float splitValue, frontMin[3], frontMax[3], backMin[3], backMax[3];

    if (FindBestSplit(trianglesRoot, numTriangles, &splitAxis, &splitValue, min, max,
        &frontRoot, &frontCount, frontMin, frontMax, &backRoot, &backCount, backMin, backMax))
    {
        uint32_t frontNode = AllocNode();
        uint32_t backNode = AllocNode();
        KdNode* node = &_nodes[nodeIndex];
        node->Axis = splitAxis;
        node->Value = splitValue;
        node->Front = frontNode;
        memcpy_s(node->Min, sizeof(node->Min), min, sizeof(node->Min));
        memcpy_s(node->Max, sizeof(node->Max), max, sizeof(node->Max));

        Process(frontNode, frontRoot, frontCount, frontMin, frontMax);
        Process(backNode, backRoot, backCount, backMin, backMax);
    }
    else
    {
        KdNode* node = &_nodes[nodeIndex];
        node->Axis = 3;
        node->NumTriangles = numTriangles;
        memcpy_s(node->Min, sizeof(node->Min), min, sizeof(node->Min));
        memcpy_s(node->Max, sizeof(node->Max), max, sizeof(node->Max));
        for (uint32_t i = 0; i < numTriangles; ++i)
        {
            uint32_t tri = AllocTriangle(_compilerTriangles[i]);
            if (i == 0)
            {
                node->FirstTriangle = tri;
            }
        }
    }
}

_Use_decl_annotations_
bool KdTreeCompiler2::FindBestSplit(uint32_t trianglesRoot, uint32_t numTriangles,
    uint32_t* splitAxis, float* splitValue, _In_ const float min[3], _In_ const float max[3],
    uint32_t* frontRoot, uint32_t* frontCount, float frontMin[3], float frontMax[3],
    uint32_t* backRoot, uint32_t* backCount, float backMin[3], float backMax[3])
{
    if (numTriangles < 10)
    {
        return false;
    }

    uint32_t axis;
    float bestSplit = 0.0f;
    uint32_t bestAxis = 0;
    int32_t bestScore = INT32_MAX;

    // Find best split
    for (axis = 0; axis < 3; ++axis)
    {
        int32_t score = 0;
        float value = (min[axis] + max[axis]) * 0.5f;

        uint32_t tri = trianglesRoot;
        while (tri != UINT32_MAX)
        {
            KdCompilerTriangle* t = &_compilerTriangles[tri];
            score += (t->Centroid[axis] >= value) ? 1 : -1;
            tri = t->Next;
        }

        score = abs(score);
        if (score < bestScore)
        {
            bestScore = score;
            bestSplit = value;
            bestAxis = axis;
        }
    }

    if (bestScore == (int32_t)numTriangles)
    {
        return false;
    }

    *splitAxis = bestAxis;
    *splitValue = bestSplit;

    uint32_t front = UINT32_MAX, back = UINT32_MAX;
    uint32_t nFront = 0, nBack = 0;

    // Now actually split the triangles
    uint32_t tri = trianglesRoot;
    while (tri != UINT32_MAX)
    {
        KdCompilerTriangle* t = &_compilerTriangles[tri];
        uint32_t nextTri = t->Next;

        // Remove node
        t->Next = UINT32_MAX;

        if (t->Min[bestAxis] > bestSplit)
        {
            // insert in front
            if (front == UINT32_MAX)
            {
                front = tri;
                memcpy_s(frontMin, sizeof(t->Min), t->Min, sizeof(t->Min));
                memcpy_s(frontMax, sizeof(t->Max), t->Max, sizeof(t->Max));
            }
            else
            {
                t->Next = front;
                front = tri;
                for (axis = 0; axis < 3; ++axis)
                {
                    frontMin[axis] = std::min(frontMin[axis], t->Min[axis]);
                    frontMax[axis] = std::max(frontMax[axis], t->Max[axis]);
                }
            }

            ++nFront;
        }
        else if (t->Max[bestAxis] < bestSplit)
        {
            // insert in back
            if (back == UINT32_MAX)
            {
                back = tri;
                memcpy_s(backMin, sizeof(t->Min), t->Min, sizeof(t->Min));
                memcpy_s(backMax, sizeof(t->Max), t->Max, sizeof(t->Max));
            }
            else
            {
                t->Next = back;
                back = tri;
                for (axis = 0; axis < 3; ++axis)
                {
                    backMin[axis] = std::min(backMin[axis], t->Min[axis]);
                    backMax[axis] = std::max(backMax[axis], t->Max[axis]);
                }
            }

            ++nBack;
        }
        else
        {
            // TODO: split triangle
            SplitTriangle(tri, bestAxis, bestSplit, &front, &back);
        }

        tri = nextTri;
    }

    *frontRoot = front;
    *frontCount = nFront;
    *backRoot = back;
    *backCount = nBack;

    return true;
}

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

#define PUSH(head, t) \
    _compilerTriangles[t].Next = *head; \
    *head = t; \

_Use_decl_annotations_
void KdTreeCompiler2::SplitTriangle(uint32_t triangle, uint32_t axis, float value, uint32_t* front, uint32_t* back)
{
    KdCompilerTriangle* t = &_compilerTriangles[triangle];

    // Make copies since if we grow the StaticGeometryVertex list while processing, we don't want to end up with bunk pointers
    StaticGeometryVertex v0 = _vertices[t->Indices[0]];
    StaticGeometryVertex v1 = _vertices[t->Indices[1]];
    StaticGeometryVertex v2 = _vertices[t->Indices[2]];

    // are any of the points on the plane
    float d0 = *(&v0.Position.x + axis) - value;
    float d1 = *(&v1.Position.x + axis) - value;
    float d2 = *(&v2.Position.x + axis) - value;

    if (fabsf(d0) < epsilon)
    {
        // v0 is on plane, split v1-v2 edge
        StaticGeometryVertex vNew;
        SplitEdge(v1, v2, axis, value, &vNew);

        uint32_t iNew = (uint32_t)_vertices.size();
        _vertices.push_back(vNew);

        uint32_t t1 = AllocCompilerTriangle(_vertices.data(), t->Indices[0], t->Indices[1], iNew);
        uint32_t t2 = AllocCompilerTriangle(_vertices.data(), t->Indices[0], iNew, t->Indices[2]);

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
    else if (fabsf(d1) < epsilon)
    {
        // v1 is on plane
        StaticGeometryVertex vNew;
        SplitEdge(v0, v2, axis, value, &vNew);

        uint32_t iNew = (uint32_t)_vertices.size();
        _vertices.push_back(vNew);

        uint32_t t1 = AllocCompilerTriangle(_vertices.data(), t->Indices[0], t->Indices[1], iNew);
        uint32_t t2 = AllocCompilerTriangle(_vertices.data(), t->Indices[1], t->Indices[2], iNew);

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
    else if (fabsf(d2) < epsilon)
    {
        // v2 is on plane
        StaticGeometryVertex vNew;
        SplitEdge(v0, v1, axis, value, &vNew);

        uint32_t iNew = (uint32_t)_vertices.size();
        _vertices.push_back(vNew);

        uint32_t t1 = AllocCompilerTriangle(_vertices.data(), t->Indices[0], iNew, t->Indices[2]);
        uint32_t t2 = AllocCompilerTriangle(_vertices.data(), iNew, t->Indices[1], t->Indices[2]);

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
        if (d0 > -epsilon) // v0 is in front of plane
        {
            if (d1 > -epsilon) // v1 is in front of plane
            {
                // v2 is by itself
                StaticGeometryVertex vNew1, vNew2;
                SplitEdge(v2, v0, axis, value, &vNew1);
                SplitEdge(v2, v1, axis, value, &vNew2);

                uint32_t iNew1 = (uint32_t)_vertices.size();
                _vertices.push_back(vNew1);

                uint32_t iNew2 = (uint32_t)_vertices.size();
                _vertices.push_back(vNew2);

                uint32_t t1 = AllocCompilerTriangle(_vertices.data(), iNew1, iNew2, t->Indices[2]);
                uint32_t t2 = AllocCompilerTriangle(_vertices.data(), t->Indices[0], t->Indices[1], iNew2);
                uint32_t t3 = AllocCompilerTriangle(_vertices.data(), t->Indices[0], iNew2, iNew1);

                PUSH(front, t2);
                PUSH(front, t3);
                PUSH(back, t1);
            }
            else // v1 is behind plane
            {
                if (d2 > -epsilon)  // v2 is in front
                {
                    // v1 is by itself
                    StaticGeometryVertex vNew1, vNew2;
                    SplitEdge(v1, v2, axis, value, &vNew1);
                    SplitEdge(v1, v0, axis, value, &vNew2);

                    uint32_t iNew1 = (uint32_t)_vertices.size();
                    _vertices.push_back(vNew1);

                    uint32_t iNew2 = (uint32_t)_vertices.size();
                    _vertices.push_back(vNew2);

                    uint32_t t1 = AllocCompilerTriangle(_vertices.data(), iNew1, iNew2, t->Indices[1]);
                    uint32_t t2 = AllocCompilerTriangle(_vertices.data(), t->Indices[2], t->Indices[0], iNew2);
                    uint32_t t3 = AllocCompilerTriangle(_vertices.data(), t->Indices[2], iNew2, iNew1);

                    PUSH(front, t2);
                    PUSH(front, t3);
                    PUSH(back, t1);
                }
                else // v2 is behind
                {
                    // v0 is by itself
                    StaticGeometryVertex vNew1, vNew2;
                    SplitEdge(v0, v1, axis, value, &vNew1);
                    SplitEdge(v0, v2, axis, value, &vNew2);

                    uint32_t iNew1 = (uint32_t)_vertices.size();
                    _vertices.push_back(vNew1);

                    uint32_t iNew2 = (uint32_t)_vertices.size();
                    _vertices.push_back(vNew2);

                    uint32_t t1 = AllocCompilerTriangle(_vertices.data(), iNew1, iNew2, t->Indices[0]);
                    uint32_t t2 = AllocCompilerTriangle(_vertices.data(), t->Indices[1], t->Indices[2], iNew2);
                    uint32_t t3 = AllocCompilerTriangle(_vertices.data(), t->Indices[1], iNew2, iNew1);

                    PUSH(back, t2);
                    PUSH(back, t3);
                    PUSH(front, t1);
                }
            }
        }
        else // v0 is behind plane
        {
            if (d1 < epsilon)  // v1 is behind
            {
                // v2 is by itself
                StaticGeometryVertex vNew1, vNew2;
                SplitEdge(v2, v0, axis, value, &vNew1);
                SplitEdge(v2, v1, axis, value, &vNew2);

                uint32_t iNew1 = (uint32_t)_vertices.size();
                _vertices.push_back(vNew1);

                uint32_t iNew2 = (uint32_t)_vertices.size();
                _vertices.push_back(vNew2);

                uint32_t t1 = AllocCompilerTriangle(_vertices.data(), iNew1, iNew2, t->Indices[2]);
                uint32_t t2 = AllocCompilerTriangle(_vertices.data(), t->Indices[0], t->Indices[1], iNew2);
                uint32_t t3 = AllocCompilerTriangle(_vertices.data(), t->Indices[0], iNew2, iNew1);

                PUSH(back, t2);
                PUSH(back, t3);
                PUSH(front, t1);
            }
            else // v1 is in front
            {
                if (d2 < epsilon) // v2 is behind
                {
                    // v1 is by itself
                    StaticGeometryVertex vNew1, vNew2;
                    SplitEdge(v1, v2, axis, value, &vNew1);
                    SplitEdge(v1, v0, axis, value, &vNew2);

                    uint32_t iNew1 = (uint32_t)_vertices.size();
                    _vertices.push_back(vNew1);

                    uint32_t iNew2 = (uint32_t)_vertices.size();
                    _vertices.push_back(vNew2);

                    uint32_t t1 = AllocCompilerTriangle(_vertices.data(), iNew1, iNew2, t->Indices[1]);
                    uint32_t t2 = AllocCompilerTriangle(_vertices.data(), t->Indices[2], t->Indices[0], iNew2);
                    uint32_t t3 = AllocCompilerTriangle(_vertices.data(), t->Indices[2], iNew2, iNew1);

                    PUSH(back, t2);
                    PUSH(back, t3);
                    PUSH(front, t1);
                }
                else
                {
                    // v0 is by itself
                    StaticGeometryVertex vNew1, vNew2;
                    SplitEdge(v0, v1, axis, value, &vNew1);
                    SplitEdge(v0, v2, axis, value, &vNew2);

                    uint32_t iNew1 = (uint32_t)_vertices.size();
                    _vertices.push_back(vNew1);

                    uint32_t iNew2 = (uint32_t)_vertices.size();
                    _vertices.push_back(vNew2);

                    uint32_t t1 = AllocCompilerTriangle(_vertices.data(), iNew1, iNew2, t->Indices[0]);
                    uint32_t t2 = AllocCompilerTriangle(_vertices.data(), t->Indices[1], t->Indices[2], iNew2);
                    uint32_t t3 = AllocCompilerTriangle(_vertices.data(), t->Indices[1], iNew2, iNew1);

                    PUSH(front, t2);
                    PUSH(front, t3);
                    PUSH(back, t1);
                }
            }
        }
    }
}

_Use_decl_annotations_
void KdTreeCompiler2::SplitEdge(_In_ const StaticGeometryVertex& v0, _In_ const StaticGeometryVertex& v1, uint32_t axis, float value, _Out_ StaticGeometryVertex* v)
{
    XMFLOAT3 vv0 = v0.Position;
    XMFLOAT3 vv1 = v1.Position;

    float d0 = *(&vv0.x + axis) - value;

    XMVECTOR sub = XMVectorSubtract(XMLoadFloat3(&vv1), XMLoadFloat3(&vv0));
    XMVECTOR dir = XMVector3Normalize(sub);
    XMVECTOR n = XMLoadFloat3(&_normals[axis]);
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
    XMStoreFloat3(&v->Position, XMVectorAdd(XMLoadFloat3(&vv0), XMVectorScale(dir, x)));
}

void KdTreeCompiler2::AppendVisibleIndices(const float pos[3], const float min[3], const float max[3], uint32_t nodeIndex, uint32_t* indices, uint32_t maxIndices, uint32_t* numIndices, uint32_t* materials, uint32_t maxMaterials, uint32_t* numMaterials)
{
    if (*numIndices + 3 > maxIndices)
    {
        // No more room
        return;
    }

    KdNode* node = &_nodes[nodeIndex];

    // Check bounds
    //for (uint32_t axis = 0; axis < 3; ++axis)
    //{
    //    if (min[axis] > node->Max[axis] || node->Min[axis] > max[axis])
    //    {
    //        return;
    //    }
    //}

    if (node->Axis < 3)
    {
        // node
        if (pos[node->Axis] >= node->Value)
        {
            // front first, then back
            AppendVisibleIndices(pos, min, max, node->Front, indices, maxIndices, numIndices, materials, maxMaterials, numMaterials);
            AppendVisibleIndices(pos, min, max, node->Front + 1, indices, maxIndices, numIndices, materials, maxMaterials, numMaterials);
        }
        else
        {
            AppendVisibleIndices(pos, min, max, node->Front + 1, indices, maxIndices, numIndices, materials, maxMaterials, numMaterials);
            AppendVisibleIndices(pos, min, max, node->Front, indices, maxIndices, numIndices, materials, maxMaterials, numMaterials);
        }
    }
    else
    {
        // leaf
        for (uint32_t i = node->FirstTriangle; i < node->FirstTriangle + node->NumTriangles; ++i)
        {
            KdTriangle* t = &_triangles[i];
            indices[*numIndices] = t->i0;
            indices[*numIndices + 1] = t->i0 + t->i1Offset;
            indices[*numIndices + 2] = t->i0 + t->i2Offset;
            *numIndices = *numIndices + 3;

            if (*numMaterials < maxMaterials)
            {
                materials[*numMaterials] = t->MaterialId;
                *numMaterials = *numMaterials + 1;
            }
        }
    }
}

