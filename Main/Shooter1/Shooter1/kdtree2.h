#pragma once

class KdTreeCompiler2
{
    struct KdCompilerTriangle
    {
        KdCompilerTriangle(const StaticGeometryVertex* vertices, uint32_t i0, uint32_t i1, uint32_t i2) :
            Next(UINT32_MAX)
        {
            Indices[0] = i0;
            Indices[1] = i1;
            Indices[2] = i2;

            const StaticGeometryVertex& v0 = vertices[i0];
            const StaticGeometryVertex& v1 = vertices[i1];
            const StaticGeometryVertex& v2 = vertices[i2];

            static const float oneThird = 1.0f / 3.0f;
            for (int i = 0; i < 3; ++i)
            {
                Min[i] = std::min(std::min(*(&v0.Position.x + i), *(&v1.Position.x + i)), *(&v2.Position.x + i));
                Max[i] = std::max(std::max(*(&v0.Position.x + i), *(&v1.Position.x + i)), *(&v2.Position.x + i));
                Centroid[i] = (*(&v0.Position.x + i) + *(&v1.Position.x + i) + *(&v2.Position.x + i)) * oneThird;
            }

            MaterialId = v0.MaterialId;
        }

        uint32_t MaterialId;
        uint32_t Indices[3];
        float Min[3];
        float Max[3];
        float Centroid[3];
        uint32_t Next;
    };

    struct KdNode
    {
        uint8_t Axis : 3;   // 0 = x, 1 = y, 2 = z, 3 = leaf
        union
        {
            // If node
            struct
            {
                float Value;
                uint32_t Front; // Back child is always immediately after
            };

            // If leaf
            struct
            {
                uint32_t FirstTriangle;
                uint32_t NumTriangles;
            };
        };
        float Min[3];
        float Max[3];
    };

    struct KdTriangle
    {
        KdTriangle(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t materialId) :
            i0(i0), i1Offset((int8_t)(i1 - i0)), i2Offset((int8_t)(i2 - i0)), MaterialId(materialId)
        {
            assert(labs((long)(i1) - i0) < 128);
            assert(labs((long)(i2) - i0) < 128);
        }

        uint32_t MaterialId;
        uint32_t i0;
        int8_t i1Offset;
        int8_t i2Offset;
    };

public:
    static KdTreeCompiler2* CreateFromTriangles(_In_count_(numVertices) const StaticGeometryVertex* vertices, _In_ uint32_t numVertices, _In_count_(numIndices) const uint32_t* indices, _In_ uint32_t numIndices);

    void BuildVisibleIndexList(_In_ const XMMATRIX& cameraWorld, _In_ const XMMATRIX& projection, _Out_writes_(maxIndices) uint32_t* indices, uint32_t maxIndices, _Out_ uint32_t* numIndices, _Out_writes_(maxMaterials) uint32_t* materials, uint32_t maxMaterials, _Out_ uint32_t* numMaterials);

private:
    KdTreeCompiler2();
    KdTreeCompiler2(const KdTreeCompiler2&);
    KdTreeCompiler2& operator= (const KdTreeCompiler2&);

    uint32_t AllocNode();
    uint32_t AllocTriangle(const KdCompilerTriangle&);
    uint32_t AllocCompilerTriangle(const StaticGeometryVertex* vertices, uint32_t i0, uint32_t i1, uint32_t i2);

    void Process(uint32_t nodeIndex, uint32_t trianglesRoot, uint32_t numTriangles, _In_ const float min[3], _In_ const float max[3]);

    bool FindBestSplit(uint32_t trianglesRoot, uint32_t numTriangles,
        _Out_ uint32_t* splitAxis, _Out_ float* splitValue, _In_ const float min[3], _In_ const float max[3],
        _Out_ uint32_t* frontRoot, _Out_ uint32_t* frontCount, _Out_ float frontMin[3], _Out_ float frontMax[3],
        _Out_ uint32_t* backRoot, _Out_ uint32_t* backCount, _Out_ float backMin[3], _Out_ float backMax[3]);

    void SplitTriangle(uint32_t triangle, uint32_t axis, float value, uint32_t* front, uint32_t* back);
    void SplitEdge(_In_ const StaticGeometryVertex& v0, _In_ const StaticGeometryVertex& v1, uint32_t axis, float value, _Out_ StaticGeometryVertex* v);

    void AppendVisibleIndices(const float pos[3], const float min[3], const float max[3], uint32_t nodeIndex, uint32_t* indices, uint32_t maxIndices, uint32_t* numIndices, uint32_t* materials, uint32_t maxMaterials, uint32_t* numMaterials);

private:
    XMFLOAT3 _normals[3]; // Axis is index into this if < 3
    uint32_t _root;
    std::vector<StaticGeometryVertex> _vertices;
    std::vector<KdNode> _nodes;
    std::vector<KdTriangle> _triangles;
    std::vector<KdCompilerTriangle> _compilerTriangles;
};
