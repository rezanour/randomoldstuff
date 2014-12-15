#include "precomp.h"
#include "geopool.h"
#include "bspcompiler.h"

//
// TODO: THIS SHOULD NOT BE IN THE RUNTIME!!!! MOVE TO PIPELINE
//

#define PARALLEL_BSP

//
// Algorithm for generating Bsp
//

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
#define ABS(f) (float)((uint32_t)f & 0x7FFFFFFF)

_Use_decl_annotations_
BspCompiler* BspCompiler::CreateFromTriangles(const StaticGeometryVertex* vertices, uint32_t numVertices, const uint32_t* indices, uint32_t numIndices)
{
    UNREFERENCED_PARAMETER(numVertices);

    BspCompiler* bsp = new BspCompiler;

    uint32_t numTriangles = numIndices / 3;
    assert(numTriangles > 0);

    Triangle* triangles = new Triangle(vertices[indices[0]], vertices[indices[1]], vertices[indices[2]]);
    Triangle* head = triangles;
    for (uint32_t i = 1; i < numTriangles; ++i)
    {
        Triangle* t = new Triangle(vertices[indices[i * 3]], vertices[indices[i * 3 + 1]], vertices[indices[i * 3 + 2]]);
        head->Next = t;
        head = t;
    }

    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);

    QueryPerformanceCounter(&start);
    bsp->_root = bsp->ProcessTriangles(&triangles, numTriangles);
    QueryPerformanceCounter(&end);

    DebugOut("Time to compile BSP: %5.3f\n", (end.QuadPart - start.QuadPart) / (double)freq.QuadPart);

    return bsp;
}

BspCompiler::BspCompiler() :
    _root(0)
{
}

_Use_decl_annotations_
int32_t BspCompiler::ProcessTriangles(Triangle** triangles, uint32_t count)
{
    Triangle* front = nullptr;
    Triangle* back = nullptr;
    const Triangle* splitTriangle = nullptr;
    uint32_t frontCount, backCount;

    if (FindBestSplit(triangles, count, &splitTriangle, &front, &frontCount, &back, &backCount))
    {
        // Able to split the set of triangles. We need a node to desribe this
        Node node;
        node.Plane = splitTriangle->Plane;
        node.Front = ProcessTriangles(&front, frontCount);
        node.Back = ProcessTriangles(&back, backCount);
        _nodes.push_back(node);
        return static_cast<int32_t>(_nodes.size() - 1);
    }
    else
    {
        // Not able to split, already convex. Make it a sector
        Sector sector;
        CreateSectorFromTriangles(triangles, count, &sector);
        _sectors.push_back(sector);
        return static_cast<int32_t>(_sectors.size() - 1) * -1 - 1;
    }
}

_Use_decl_annotations_
void BspCompiler::CreateSectorFromTriangles(Triangle** triangles, uint32_t count, Sector* sector)
{
    UNREFERENCED_PARAMETER(count);
    sector->Head = *triangles;
    *triangles = nullptr;
}

DWORD CALLBACK BspCompiler::FindBestSplitTaskThreadProc(void* p)
{
    FindBestSplitPacket* packet = (FindBestSplitPacket*)p;
    FindBestSplitTask(packet);
    return 0;
}

_Use_decl_annotations_
bool BspCompiler::FindBestSplit(Triangle** triangles, uint32_t count, const Triangle** splitTriangle, Triangle** front, uint32_t* frontCount, Triangle** back, uint32_t* backCount)
{
    const Triangle* candidate = nullptr;
    int32_t score = 0;

#if defined(PARALLEL_BSP)
    if (count > 1000)
    {
        // Divide up workload and spin up multiple SplitTriangleTasks

        uint32_t numGroups = std::min((int)(count / 1000), 16);
        std::vector<std::shared_ptr<FindBestSplitPacket>> packets(numGroups + 1);
        std::vector<HANDLE> threads(numGroups + 1);

        for (uint32_t i = 0; i < packets.size(); ++i)
        {
            packets[i].reset(new FindBestSplitPacket);
        }

        uint32_t countPerGroup = count / numGroups;
        uint32_t countDispatched = 0;

        const Triangle* start = *triangles;
        for (uint32_t i = 0; i < numGroups; ++i)
        {
            auto& packet = packets[i];
            packet->start = start;
            packet->head = *triangles;
            packet->count = count;
            packet->numTriangles = countPerGroup;
            threads[i] = CreateThread(nullptr, 0, FindBestSplitTaskThreadProc, packet.get(), 0, nullptr);

            for (uint32_t j = 0; j < countPerGroup; ++j)
            {
                start = start->Next;
            }

            countDispatched += countPerGroup;
        }

        if (countDispatched < count)
        {
            auto& packet = packets[numGroups];
            packet->start = start;
            packet->head = *triangles;
            packet->count = count;
            packet->numTriangles = count - countDispatched;
            threads[numGroups] = CreateThread(nullptr, 0, FindBestSplitTaskThreadProc, packet.get(), 0, nullptr);
            ++numGroups;
        }

        WaitForMultipleObjects(numGroups, threads.data(), TRUE, INFINITE);

        for (uint32_t i = 0; i < numGroups; ++i)
        {
            CloseHandle(threads[i]);
        }

        // aggregate best
        const Triangle* best = nullptr;
        int32_t bestScore = 0;

        for (uint32_t i = 0; i < numGroups; ++i)
        {
            if (packets[i]->result)
            {
                if (best == nullptr || packets[i]->candidateScore < bestScore)
                {
                    best = packets[i]->candidateTriangle;
                    bestScore = packets[i]->candidateScore;
                }
            }
        }

        if (best == nullptr)
        {
            return false;
        }

        candidate = best;
        score = bestScore;
    }
    else
#endif
    {
        FindBestSplitPacket packet;
        packet.head = *triangles;
        packet.count = count;
        packet.start = *triangles;
        packet.numTriangles = count;
        FindBestSplitTask(&packet);
        if (!packet.result)
        {
            return false;
        }

        candidate = packet.candidateTriangle;
        score = packet.candidateScore;
    }

    *splitTriangle = candidate;
    const XMFLOAT4& plane = candidate->Plane;

    (*frontCount) = 0;
    (*backCount) = 0;

    Triangle* t = *triangles;
    while (t != nullptr)
    {
        Triangle* next = t->Next;

        if (t == candidate)
        {
            // self. insert in front
            PUSH(front, t);
            ++(*frontCount);
        }
        else
        {
            float d0 = DistToPlane(plane, t->v0.Position);
            float d1 = DistToPlane(plane, t->v1.Position);
            float d2 = DistToPlane(plane, t->v2.Position);

            if (ABS(d0) < epsilon && ABS(d1) < epsilon && ABS(d2) < epsilon)
            {
                XMVECTOR tplane = XMLoadFloat4(&t->Plane);

                // coplanar case
                if (XMVectorGetX(XMVector3Dot(XMLoadFloat4(&plane), tplane)) > 0)
                {
                    // same facing direction
                    PUSH(front, t);
                    ++(*frontCount);
                }
                else
                {
                     PUSH(back, t);
                    ++(*backCount);
                }
            }
            else if (d0 > -epsilon && d1 > -epsilon && d2 > -epsilon)
            {
                // front of plane
                PUSH(front, t);
                ++(*frontCount);
            }
            else if (d0 < epsilon && d1 < epsilon && d2 < epsilon)
            {
                // fully behind plane
                PUSH(back, t);
                ++(*backCount);
            }
            else
            {
                // intersect
                SplitTriangle(t, plane, front, back);
            }
        }

        t = next;
    }

    return true;
}

_Use_decl_annotations_
void BspCompiler::FindBestSplitTask(FindBestSplitPacket* packet)
{
    const Triangle* head = packet->head;
    uint32_t count = packet->count;
    const Triangle* start = packet->start;
    uint32_t numTriangles = packet->numTriangles;
    const Triangle** candidateTriangle = &packet->candidateTriangle;
    int32_t* candidateScore = &packet->candidateScore;

    const Triangle* best = nullptr;
    int32_t bestScore = 0;
    int32_t score = 0;

    *candidateTriangle = nullptr;

    const Triangle* t = start;
    for (uint32_t i = 0; i < numTriangles; ++i)
    {
        const XMFLOAT4& plane = t->Plane;
        const Triangle* other = head;
        score = 0;

        for (uint32_t j = 0; j < count; ++j)
        {
            if (t == other)
            {
                // self goes in front
                ++score;
            }
            else
            {
                float d0 = DistToPlane(plane, other->v0.Position);
                float d1 = DistToPlane(plane, other->v1.Position);
                float d2 = DistToPlane(plane, other->v2.Position);

                if (ABS(d0) < epsilon && ABS(d1) < epsilon && ABS(d2) < epsilon)
                {
                    XMVECTOR otherPlane = XMLoadFloat4(&other->Plane);

                    // coplanar case
                    if (XMVectorGetX(XMVector3Dot(XMLoadFloat4(&plane), otherPlane)) > 0)
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

            other = other->Next;
        }

        score = abs(score);

        if (best == nullptr || score < bestScore)
        {
            best = t;
            bestScore = score;
        }

        t = t->Next;
    }

    if (best != nullptr && bestScore != (int32_t)count)
    {
        *candidateTriangle = best;
        *candidateScore = bestScore;
        packet->result = true;
    }

    packet->result = false;
}

_Use_decl_annotations_
void BspCompiler::SplitTriangle(const Triangle* t, const XMFLOAT4& plane, Triangle** front, Triangle** back)
{
    // are any of the points on the plane
    float d0 = DistToPlane(plane, t->v0.Position);
    float d1 = DistToPlane(plane, t->v1.Position);
    float d2 = DistToPlane(plane, t->v2.Position);

    if (ABS(d0) < epsilon)
    {
        // v0 is on plane, split v1-v2 edge
        Vertex vNew;
        SplitEdge(t->v1, t->v2, plane, &vNew);

        Triangle* t1 = new Triangle(t->v0, t->v1, vNew);
        Triangle* t2 = new Triangle(t->v0, vNew, t->v2);

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
        // v1 is on plane
        Vertex vNew;
        SplitEdge(t->v0, t->v2, plane, &vNew);

        Triangle* t1 = new Triangle(t->v0, t->v1, vNew);
        Triangle* t2 = new Triangle(t->v1, t->v2, vNew);

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
        // v2 is on plane
        Vertex vNew;
        SplitEdge(t->v0, t->v1, plane, &vNew);

        Triangle* t1 = new Triangle(t->v0, vNew, t->v2);
        Triangle* t2 = new Triangle(vNew, t->v1, t->v2);

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
                Vertex vNew1, vNew2;
                SplitEdge(t->v2, t->v0, plane, &vNew1);
                SplitEdge(t->v2, t->v1, plane, &vNew2);

                Triangle* t1 = new Triangle(vNew1, vNew2, t->v2);
                Triangle* t2 = new Triangle(t->v0, t->v1, vNew2);
                Triangle* t3 = new Triangle(t->v0, vNew2, vNew1);

                PUSH(front, t2);
                PUSH(front, t3);
                PUSH(back, t1);
            }
            else // v1 is behind plane
            {
                if (d2 > -epsilon)  // v2 is in front
                {
                    // v1 is by itself
                    Vertex vNew1, vNew2;
                    SplitEdge(t->v1, t->v2, plane, &vNew1);
                    SplitEdge(t->v1, t->v0, plane, &vNew2);

                    Triangle* t1 = new Triangle(vNew1, vNew2, t->v1);
                    Triangle* t2 = new Triangle(t->v2, t->v0, vNew2);
                    Triangle* t3 = new Triangle(t->v2, vNew2, vNew1);

                    PUSH(front, t2);
                    PUSH(front, t3);
                    PUSH(back, t1);
                }
                else // v2 is behind
                {
                    // v0 is by itself
                    Vertex vNew1, vNew2;
                    SplitEdge(t->v0, t->v1, plane, &vNew1);
                    SplitEdge(t->v0, t->v2, plane, &vNew2);

                    Triangle* t1 = new Triangle(vNew1, vNew2, t->v0);
                    Triangle* t2 = new Triangle(t->v1, t->v2, vNew2);
                    Triangle* t3 = new Triangle(t->v1, vNew2, vNew1);

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
                Vertex vNew1, vNew2;
                SplitEdge(t->v2, t->v0, plane, &vNew1);
                SplitEdge(t->v2, t->v1, plane, &vNew2);

                Triangle* t1 = new Triangle(vNew1, vNew2, t->v2);
                Triangle* t2 = new Triangle(t->v0, t->v1, vNew2);
                Triangle* t3 = new Triangle(t->v0, vNew2, vNew1);

                PUSH(back, t2);
                PUSH(back, t3);
                PUSH(front, t1);
            }
            else // v1 is in front
            {
                if (d2 < epsilon) // v2 is behind
                {
                    // v1 is by itself
                    Vertex vNew1, vNew2;
                    SplitEdge(t->v1, t->v2, plane, &vNew1);
                    SplitEdge(t->v1, t->v0, plane, &vNew2);

                    Triangle* t1 = new Triangle(vNew1, vNew2, t->v1);
                    Triangle* t2 = new Triangle(t->v2, t->v0, vNew2);
                    Triangle* t3 = new Triangle(t->v2, vNew2, vNew1);

                    PUSH(back, t2);
                    PUSH(back, t3);
                    PUSH(front, t1);
                }
                else
                {
                    // v0 is by itself
                    Vertex vNew1, vNew2;
                    SplitEdge(t->v0, t->v1, plane, &vNew1);
                    SplitEdge(t->v0, t->v2, plane, &vNew2);

                    Triangle* t1 = new Triangle(vNew1, vNew2, t->v0);
                    Triangle* t2 = new Triangle(t->v1, t->v2, vNew2);
                    Triangle* t3 = new Triangle(t->v1, vNew2, vNew1);

                    PUSH(front, t2);
                    PUSH(front, t3);
                    PUSH(back, t1);
                }
            }
        }
    }
}

_Use_decl_annotations_
void BspCompiler::SplitEdge(const Vertex& v0, const Vertex& v1, const XMFLOAT4& plane, Vertex* v)
{
    XMFLOAT3 vv0 = v0.Position;
    XMFLOAT3 vv1 = v1.Position;

    float d0 = DistToPlane(plane, vv0);

    XMVECTOR sub = XMVectorSubtract(XMLoadFloat3(&vv1), XMLoadFloat3(&vv0));
    XMVECTOR dir = XMVector3Normalize(sub);
    XMVECTOR n = XMLoadFloat4(&plane);
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
