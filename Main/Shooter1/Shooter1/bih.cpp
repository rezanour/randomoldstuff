#include "precomp.h"
#include "bih.h"
#include <DirectXCollision.h>

#define PUSH(pHead, t) t->Next = *pHead; *pHead = t;

_Use_decl_annotations_
BIH* BIH::CreateFromTriangles(const StaticGeometryVertex* vertices, uint32_t numVertices, const uint32_t* indices, uint32_t numIndices)
{
    UNREFERENCED_PARAMETER(numVertices);

    uint32_t numTriangles = numIndices / 3;
    assert(numTriangles > 0);

    CompileTriangle* triangles = new CompileTriangle(vertices[indices[0]], vertices[indices[1]], vertices[indices[2]], indices[0], indices[1], indices[2], vertices[indices[0]].MaterialId);
    XMFLOAT3 min = triangles->Min;
    XMFLOAT3 max = triangles->Max;

    for (uint32_t i = 1; i < numTriangles; ++i)
    {
        CompileTriangle* t = new CompileTriangle(vertices[indices[i * 3]], vertices[indices[i * 3 + 1]], vertices[indices[i * 3 + 2]],
                                                 indices[i * 3], indices[i * 3 + 1], indices[i * 3 + 2], vertices[indices[i * 3]].MaterialId);
        t->Next = triangles;
        triangles = t;

        min = VecMin(min, t->Min);
        max = VecMax(max, t->Max);
    }

    BIH* bih = new BIH;
    bih->_bounds = AABB(min, max);
    bih->_root = bih->ProcessTriangles(&triangles, numTriangles, bih->_bounds);

    return bih;
}

BIH::BIH() :
    _root((uint32_t)-1)
{
}

BIH::~BIH()
{
}

_Use_decl_annotations_
uint32_t BIH::ProcessTriangles(CompileTriangle** triangles, uint32_t count, const AABB& bounds)
{
    assert(*triangles != nullptr);
    assert(count > 0);

    if (count < 4)
    {
        // Make a leaf
        Node node;
        node.Axis = 3;

        node.StartTriangle = (uint32_t)_triangles.size();
        CompileTriangle* t = *triangles;
        while (t != nullptr)
        {
            CompileTriangle* next = t->Next;
            _triangles.push_back(Triangle(t));
            delete t;
            t = next;
        }
        *triangles = nullptr;

        node.NumTriangles = (uint32_t)_triangles.size() - node.StartTriangle;

        _nodes.push_back(node);
        return (uint32_t)_nodes.size() - 1;
    }
    else
    {
        // Split

        // Choose longest axis as initial candidate, but we may end up
        // trying all three if we can't find a good one
        uint8_t axis = 0;
        float xLength = bounds.GetMax().x - bounds.GetMin().x;
        float yLength = bounds.GetMax().y - bounds.GetMin().y;
        float zLength = bounds.GetMax().z - bounds.GetMin().z;
        if (yLength > xLength)
        {
            axis = 1;
            if (zLength > yLength)
            {
                axis = 2;
            }
        }
        else if (zLength > xLength)
        {
            axis = 2;
        }

        uint8_t startAxis = axis;

        CompileTriangle* t;
        uint32_t minCount;
        uint32_t maxCount;

        // Loop and test each axis. Doesn't actually modify lists yet, just counting
        for (;;)
        {
            t = *triangles;

            float totalAlongAxis = 0;
            while (t != nullptr)
            {
                totalAlongAxis += *(&t->Centroid.x + axis);
                t = t->Next;
            }

            float averageAlongAxis = totalAlongAxis / (float)count;

            t = *triangles;
            minCount = 0;
            maxCount = 0;

            while (t != nullptr)
            {
                float v = *(&t->Centroid.x + axis);
                if (v < averageAlongAxis)
                {
                    ++minCount;
                }
                else
                {
                    ++maxCount;
                }

                t = t->Next;
            }

            if (minCount == 0 || maxCount == 0)
            {
                // try the next axis if this one wasn't any good
                axis = (axis + 1) % 3;
                if (axis == startAxis)
                {
                    // bail, no good
                    break;
                }
            }
            else
            {
                // this axis is fine.
                break;
            }
        };

        if (minCount == 0 || maxCount == 0)
        {
            // Failed to split, just make it a leaf
            Node node;
            node.Axis = 3;

            node.StartTriangle = (uint32_t)_triangles.size();
            CompileTriangle* t = *triangles;
            while (t != nullptr)
            {
                CompileTriangle* next = t->Next;
                _triangles.push_back(Triangle(t));
                delete t;
                t = next;
            }
            *triangles = nullptr;

            node.NumTriangles = (uint32_t)_triangles.size() - node.StartTriangle;

            _nodes.push_back(node);
            return (uint32_t)_nodes.size() - 1;
        }
        else
        {
            // Now we need to actually build lists
            CompileTriangle* minTriangles = nullptr;
            CompileTriangle* maxTriangles = nullptr;
            float min = FLT_MAX;
            float max = -FLT_MAX;

            minCount = 0;
            maxCount = 0;

            t = *triangles;

            float totalAlongAxis = 0;
            while (t != nullptr)
            {
                totalAlongAxis += *(&t->Centroid.x + axis);
                t = t->Next;
            }

            float averageAlongAxis = totalAlongAxis / (float)count;

            t = *triangles;
            while (t != nullptr)
            {
                CompileTriangle* next = t->Next;

                float v = *(&t->Centroid.x + axis);
                if (v < averageAlongAxis)
                {
                    max = std::max(max, *(&t->Max.x + axis));
                    PUSH(&minTriangles, t);
                    ++minCount;
                }
                else
                {
                    min = std::min(min, *(&t->Min.x + axis));
                    PUSH(&maxTriangles, t);
                    ++maxCount;
                }

                t = next;
            }

            Node node;
            node.Axis = axis;
            node.Max = max;
            node.Min = min;

            _nodes.push_back(node);

            uint32_t index = (uint32_t)_nodes.size() - 1;

            XMFLOAT3 boundsSide = bounds.GetMax();
            *(&boundsSide.x + axis) = max;
            uint32_t minNode = ProcessTriangles(&minTriangles, minCount, AABB(bounds.GetMin(), boundsSide));
            DBG_UNREFERENCED_LOCAL_VARIABLE(minNode);
            assert(minNode == index + 1);

            boundsSide = bounds.GetMin();
            *(&boundsSide.x + axis) = min;
            _nodes[index].MaxNode = ProcessTriangles(&maxTriangles, maxCount, AABB(boundsSide, bounds.GetMax()));

            return index;
        }
    }
}

_Use_decl_annotations_
void BIH::BruteForceQuery(uint32_t* numTriangles)
{
    *numTriangles = (uint32_t)_triangles.size();
}

_Use_decl_annotations_
void BIH::Query(const AABB& test, uint32_t* numTriangles)
{
    *numTriangles = 0;
    if (!test.Intersects(_bounds))
    {
        return;
    }

    Query(test, _root, numTriangles);
}

_Use_decl_annotations_
void BIH::Query(const AABB& test, uint32_t nodeIndex, uint32_t* numTriangles)
{
    Node& node = _nodes[nodeIndex];
    if (node.Axis == 3)
    {
        // Leaf
        *numTriangles += node.NumTriangles;
    }
    else
    {
        if (*(&test.GetMin().x + node.Axis) < node.Max)
        {
            Query(test, nodeIndex + 1, numTriangles);
        }

        if (*(&test.GetMax().x + node.Axis) > node.Min)
        {
            Query(test, node.MaxNode, numTriangles);
        }
    }
}

_Use_decl_annotations_
void BIH::BuildVisibleIndexList(const XMMATRIX& cameraWorld, const XMMATRIX& projection, uint32_t* indices, uint32_t maxIndices, uint32_t* numIndices, uint32_t* materials, uint32_t maxMaterials, uint32_t* numMaterials)
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

    AABB bounds(min, max);

    AppendVisibleIndices(bounds, _root, indices, maxIndices, numIndices, materials, maxMaterials, numMaterials);
}

_Use_decl_annotations_
void BIH::AppendVisibleIndices(const AABB& test, uint32_t nodeIndex, uint32_t* indices, uint32_t maxIndices, uint32_t* numIndices, uint32_t* materials, uint32_t maxMaterials, uint32_t* numMaterials)
{
    if (*numIndices + 3 > maxIndices)
    {
        // No more room
        return;
    }

    Node& node = _nodes[nodeIndex];
    if (node.Axis == 3)
    {
        // Leaf
        Triangle* t = &_triangles[node.StartTriangle];
        for (uint32_t i = 0; (i < node.NumTriangles) && (*numIndices + 3 <= maxIndices); ++i, ++t)
        {
            indices[*numIndices] = t->i0;   ++(*numIndices);
            indices[*numIndices] = t->i1;   ++(*numIndices);
            indices[*numIndices] = t->i2;   ++(*numIndices);
            materials[*numMaterials] = t->MaterialId;   ++(*numMaterials);
        }
    }
    else
    {
        // Inner node
        if (*(&test.GetMin().x + node.Axis) < node.Max)
        {
            AppendVisibleIndices(test, nodeIndex + 1, indices, maxIndices, numIndices, materials, maxMaterials, numMaterials);
        }

        if (*(&test.GetMax().x + node.Axis) > node.Min)
        {
            AppendVisibleIndices(test, node.MaxNode, indices, maxIndices, numIndices, materials, maxMaterials, numMaterials);
        }
    }
}
