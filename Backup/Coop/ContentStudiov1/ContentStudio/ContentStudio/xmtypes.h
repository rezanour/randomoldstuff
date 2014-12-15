#pragma once

struct XMCollisionTriangle
{
    XMVECTOR pointa;
    XMVECTOR pointb;
    XMVECTOR pointc;
    int Collision;
};

struct XMCollisionRay
{
    XMVECTOR origin;
    XMVECTOR direction;
};

struct XMPosVertex
{
    XMFLOAT3 position; // Position
};

struct XMPosTexVertex
{
    XMFLOAT3 position; // Position
    XMFLOAT2 texCoord; // Texture coordinate
};

struct XMPosTexNormVertex
{
    XMFLOAT3 position; // Position
    XMFLOAT2 texCoord; // Texture coordinate
    XMFLOAT3 normal; // Normal
};

struct XMPosNormTexVertex
{
    XMFLOAT3 position; // Position
    XMFLOAT3 normal; // Normal
    XMFLOAT2 texCoord; // Texture coordinate
};

struct XMPosTexNormTanBiNormVertex
{
    XMFLOAT3 position; // Position
    XMFLOAT2 texCoord; // Texture coordinate
    XMFLOAT3 normal;   // Normal
    XMFLOAT3 tanget;   // Tanget
    XMFLOAT3 binormal; // BiNormal
};

struct XMPosTexNormVertexId
{
    XMPosTexNormVertex vertex;
    UINT vertexId;
};

struct XMProjSpaceVertex
{
    XMFLOAT2 position;
    XMFLOAT2 texCoord;
};


