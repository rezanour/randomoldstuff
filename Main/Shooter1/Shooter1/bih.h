#pragma once

//
// Bounding Interval Hierarchy
//

class BIH
{
    struct CompileVertex
    {
        CompileVertex() {}

        CompileVertex(_In_ const XMFLOAT3& position, _In_ const XMFLOAT3& normal, _In_ const XMFLOAT2& texCoord) :
            Position(position), Normal(normal), TexCoord(texCoord)
        {
        }

        XMFLOAT3 Position;
        XMFLOAT3 Normal;
        XMFLOAT2 TexCoord;
    };

    struct CompileTriangle : TrackedObject<MemoryTag::BIHTriangle>
    {
        CompileTriangle(_In_ const StaticGeometryVertex& a, _In_ const StaticGeometryVertex& b, _In_ const StaticGeometryVertex& c, _In_ uint32_t i0, _In_ uint32_t i1, _In_ uint32_t i2, uint32_t materialId) :
            v0(a.Position, a.Normal, a.TexCoord), v1(b.Position, b.Normal, b.TexCoord), v2(c.Position, c.Normal, c.TexCoord), Next(nullptr),
            i0(i0), i1(i1), i2(i2), MaterialId(materialId)
        {
            Init();
        }

        CompileVertex v0;
        CompileVertex v1;
        CompileVertex v2;
        XMFLOAT3 Centroid;
        XMFLOAT3 Min;
        XMFLOAT3 Max;
        uint32_t i0;
        uint32_t i1;
        uint32_t i2;
        uint32_t MaterialId;
        CompileTriangle* Next;

    private:
        void Init()
        {
            static const float oneThird = 1.0f / 3.0f;
            Centroid.x = (v0.Position.x + v1.Position.x + v2.Position.x) * oneThird;
            Centroid.y = (v0.Position.y + v1.Position.y + v2.Position.y) * oneThird;
            Centroid.z = (v0.Position.z + v1.Position.z + v2.Position.z) * oneThird;

            Min = VecMin(v0.Position, v1.Position);
            Min = VecMin(Min, v2.Position);

            Max = VecMax(v0.Position, v1.Position);
            Max = VecMax(Max, v2.Position);
        }
    };

    struct Triangle
    {
        Triangle(_In_ const CompileTriangle* t) :
            p0(t->v0.Position), p1(t->v1.Position), p2(t->v2.Position),
            i0(t->i0), i1(t->i1), i2(t->i2), MaterialId(t->MaterialId)
        {}

        // For collision
        XMFLOAT3 p0, p1, p2;

        // For rendering
        uint32_t i0, i1, i2;
        uint32_t MaterialId;
    };

    struct Node
    {
        // MinNode is always sequentially placed after this one, so index is implicit
        uint32_t MaxNode    :30;
        uint32_t Axis       : 2;

        union
        {
            struct
            {
                float Min;
                float Max;
            };
            struct
            {
                uint32_t StartTriangle;
                uint32_t NumTriangles;
            };
        };
    };

public:
    static BIH* CreateFromTriangles(_In_count_(numVertices) const StaticGeometryVertex* vertices, _In_ uint32_t numVertices, _In_count_(numIndices) const uint32_t* indices, _In_ uint32_t numIndices);
    ~BIH();

    void BruteForceQuery(_Out_ uint32_t* numTriangles);
    void Query(_In_ const AABB& test, _Out_ uint32_t* numTriangles);

    void BuildVisibleIndexList(_In_ const XMMATRIX& cameraWorld, _In_ const XMMATRIX& projection, _Out_writes_(maxIndices) uint32_t* indices, uint32_t maxIndices, _Out_ uint32_t* numIndices, _Out_writes_(maxMaterials) uint32_t* materials, uint32_t maxMaterials, _Out_ uint32_t* numMaterials);

private:
    BIH();
    BIH(const BIH&);

    uint32_t ProcessTriangles(_In_ CompileTriangle** triangles, _In_ uint32_t count, _In_ const AABB& bounds);
    void Query(_In_ const AABB& test, _In_ uint32_t nodeIndex, _Inout_ uint32_t* numTriangles);

    void AppendVisibleIndices(_In_ const AABB& test, _In_ uint32_t nodeIndex, _Inout_updates_(maxIndices) uint32_t* indices, uint32_t maxIndices, _Inout_ uint32_t* numIndices, _Inout_updates_(maxMaterials) uint32_t* materials, uint32_t maxMaterials, _Inout_ uint32_t* numMaterials);

private:
    AABB _bounds;
    std::vector<Node> _nodes;
    std::vector<Triangle> _triangles;
    uint32_t _root;
};
