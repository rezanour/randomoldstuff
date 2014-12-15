#pragma once

struct ITexture;

//
// The BSP code is designed to be a robust primitive in the system, with no
// knowledge of other components or how they will use the BSP. To this end,
// the core BSP types are made public here in this header so that other components
// such as the renderer can build their own optimal traversal code, instead of
// baking that logic into the BSP layer. This also allows efficient collision
// detection and response, and other physics code, to build on top of some
// simple raycasting and shape tracing primitives that will be exposed in the BSP
// layer.
//

struct BspVertex
{
    XMFLOAT3 Position;
};

struct BspTriangle
{
    BspTriangle(const XMFLOAT3& v0, const XMFLOAT3& v1, const XMFLOAT3& v2)
    {
        V0.Position = v0;
        V1.Position = v1;
        V2.Position = v2;
        XMVECTOR normal = XMVector3Normalize(XMVector3Cross(XMVectorSubtract(XMLoadFloat3(&v1), XMLoadFloat3(&v0)), XMVectorSubtract(XMLoadFloat3(&v2), XMLoadFloat3(&v0))));
        // plane's xyz are the xyz of the normal, and it's w is the dot of v0 & n
        XMStoreFloat4(&Plane, XMVectorSelect(normal, XMVector3Dot(XMLoadFloat3(&V0.Position), normal), XMVectorSelectControl(0, 0, 0, 1)));
    }

    BspVertex V0, V1, V2;
    XMFLOAT4 Plane;
};

struct BspNode
{
    XMFLOAT4 Plane;
    int32_t Front;
    int32_t Back;
};

struct BspSector
{
    uint32_t FirstTriangle;
    uint32_t NumTriangles;
};

struct BspData
{
    int32_t Root;

    std::vector<BspNode> Nodes;
    std::vector<BspSector> Sectors;
    std::vector<BspTriangle> Triangles;
};

struct ClipResult
{
    float Dist;
    XMFLOAT4 Plane; // plane that caused the movement to be clipped. Can be used for collision response
};

struct __declspec(novtable) IBsp
{
    // For raw access, allowing other components to build custom traversal or access code
    virtual const BspData& GetData() const = 0;

    //
    // Casting/clipping routines
    //

    // Clip a line segment against the bsp. This can be used for ray casts and line of site queries
    _Success_(return)
    virtual bool ClipLineSegment(_In_ const XMVECTOR& start, _In_ const XMVECTOR& end, _Out_ ClipResult& result) const = 0;

    // Clip the movement of an ellipsoid
    virtual void CheckEllipsoid(_Inout_ CollisionQuery& query) const = 0;
};

// NOTE: The triangle list passed into this method is destructively consumed by the method and is not valid after the call.
std::shared_ptr<IBsp> GenerateBspFromTriangles(_Inout_ std::vector<BspTriangle>& triangles);
