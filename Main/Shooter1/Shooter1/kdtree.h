#pragma once

class KdTreeCompiler
{
    struct KdCompilerTriangle
    {
        ~KdCompilerTriangle()
        {
            if (Next)
            {
                delete Next;
            }
        }

        KdCompilerTriangle(const StaticGeometryVertex& v0, const StaticGeometryVertex& v1, const StaticGeometryVertex& v2) : V0(v0), V1(v1), V2(v2), Next(nullptr)
        {
            Min = VecMin(v0.Position, VecMin(v1.Position, v2.Position));
            Max = VecMax(v0.Position, VecMax(v1.Position, v2.Position));
            Centroid = XMFLOAT3(v0.Position.x + v1.Position.x + v2.Position.x, v0.Position.y + v1.Position.y + v2.Position.y, v0.Position.z + v1.Position.z + v2.Position.z);
            assert(!isnan(Centroid.x));
            assert(!isnan(Centroid.y));
            assert(!isnan(Centroid.z));
            Centroid.x /= 3.0f;
            Centroid.y /= 3.0f;
            Centroid.z /= 3.0f;
        }

        StaticGeometryVertex V0, V1, V2;
        XMFLOAT3 Centroid;
        XMFLOAT3 Min;
        XMFLOAT3 Max;
        KdCompilerTriangle* Next;
    };

    struct KdCompilerNode
    {
        ~KdCompilerNode()
        {
            if (Axis == 3)
            {
                delete Head;
            }
            else
            {
                delete Front;
                delete Back;
            }
        }

        uint32_t Axis : 3;
        float Value;

        // If Axis == 3, then this is a sector. Use Head
        union
        {
            KdCompilerNode* Front;
            KdCompilerTriangle* Head;
        };

        KdCompilerNode* Back;
    };

public:
    static KdTreeCompiler* CreateFromTriangles(_In_count_(numVertices) const StaticGeometryVertex* vertices, _In_ uint32_t numVertices, _In_count_(numIndices) const uint32_t* indices, _In_ uint32_t numIndices);
    ~KdTreeCompiler();

private:
    KdTreeCompiler();
    KdTreeCompiler(const KdTreeCompiler&);
    KdTreeCompiler& operator= (const KdTreeCompiler&);

    KdCompilerNode* Process(_Inout_ KdCompilerTriangle** triangles, _In_ const AABB& bounds);
    bool SplitTriangles(_Inout_ KdCompilerTriangle** triangles, _In_ const AABB& bounds, _Outptr_ KdCompilerTriangle** front, _Out_ AABB* frontBounds, _Outptr_ KdCompilerTriangle** back, _Out_ AABB* backBounds, _Out_ uint32_t* axis, _Out_ float* value);

    void SplitTriangle(const KdCompilerTriangle* t, const XMFLOAT4& plane, KdCompilerTriangle** front, KdCompilerTriangle** back);
    void SplitEdge(const StaticGeometryVertex& v0, const StaticGeometryVertex& v1, const XMFLOAT4& plane, StaticGeometryVertex* v);

private:
    XMFLOAT3 _normals[3]; // Axis is index into this if < 3
    KdCompilerNode* _root;
};
