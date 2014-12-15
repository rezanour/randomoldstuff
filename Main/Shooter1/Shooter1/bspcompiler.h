#pragma once

#include <ppl.h>
#include <ppltasks.h>

//
// TODO: THIS IS IN THE RUNTIME FOR TESTING ONLY. WILL BE MOVED SOON
//
class BspCompiler
{
    struct Vertex
    {
        Vertex() {}

        Vertex(_In_ const XMFLOAT3& position, _In_ const XMFLOAT3& normal, _In_ const XMFLOAT2& texCoord) :
            Position(position), Normal(normal), TexCoord(texCoord)
        {
        }

        XMFLOAT3 Position;
        XMFLOAT3 Normal;
        XMFLOAT2 TexCoord;
    };

    struct Triangle
    {
        Triangle(_In_ const StaticGeometryVertex& a, _In_ const StaticGeometryVertex& b, _In_ const StaticGeometryVertex& c) :
            v0(a.Position, a.Normal, a.TexCoord), v1(b.Position, b.Normal, b.TexCoord), v2(c.Position, c.Normal, c.TexCoord), Next(nullptr)
        {
            ComputePlane();
        }

        Triangle(_In_ const Vertex& a, _In_ const Vertex& b, _In_ const Vertex& c) :
            v0(a), v1(b), v2(c), Next(nullptr)
        {
            ComputePlane();
        }

        Vertex v0;
        Vertex v1;
        Vertex v2;
        XMFLOAT4 Plane;
        Triangle* Next;

    private:
        void ComputePlane()
        {
            XMVECTOR normal = XMVector3Normalize(XMVector3Cross(
                XMVectorSubtract(XMLoadFloat3(&v1.Position), XMLoadFloat3(&v0.Position)),
                XMVectorSubtract(XMLoadFloat3(&v2.Position), XMLoadFloat3(&v0.Position))));

            XMStoreFloat4(&Plane, XMVectorSetW(normal, XMVectorGetX(XMVector3Dot(normal, XMLoadFloat3(&v0.Position)))));
        }
    };

    struct Node
    {
        XMFLOAT4 Plane;
        // positive value indicates node, negative indicates sector (index * -1 -1 to get sector index)
        int32_t Front;
        int32_t Back;
    };

    struct Sector
    {
        Triangle* Head;
    };

    struct FindBestSplitPacket
    {
        bool result;
        const Triangle* head;
        uint32_t count;
        const Triangle* start;
        uint32_t numTriangles;
        const Triangle* candidateTriangle;
        int32_t candidateScore;
    };

public:
    static BspCompiler* CreateFromTriangles(_In_count_(numVertices) const StaticGeometryVertex* vertices, _In_ uint32_t numVertices, _In_count_(numIndices) const uint32_t* indices, _In_ uint32_t numIndices);

private:
    BspCompiler();
    BspCompiler(const BspCompiler&);

    int32_t ProcessTriangles(_Inout_ Triangle** triangles, uint32_t count);
    void CreateSectorFromTriangles(_Inout_ Triangle** triangles, uint32_t count, _Out_ Sector* sector);

    // Returns false if already convex (won't split then)
    bool FindBestSplit(_Inout_ Triangle** triangles, uint32_t count, _Out_ const Triangle** splitTriangle, _Out_ Triangle** front, _Out_ uint32_t* frontCount, _Out_ Triangle** back, _Out_ uint32_t* backCount);
    static DWORD CALLBACK FindBestSplitTaskThreadProc(void* p);
    static void FindBestSplitTask(FindBestSplitPacket* packet);

    void SplitTriangle(_In_ const Triangle* t, _In_ const XMFLOAT4& plane, _Inout_ Triangle** front, _Inout_ Triangle** back);
    static void SplitEdge(_In_ const Vertex& v0, _In_ const Vertex& v1, _In_ const XMFLOAT4& plane, _Out_ Vertex* v);

private:
    std::vector<Node> _nodes;
    std::vector<Sector> _sectors;
    int32_t _root;
};
