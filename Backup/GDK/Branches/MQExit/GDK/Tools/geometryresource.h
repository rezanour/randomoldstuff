#pragma once

#include <Windows.h>
#include <DirectXTK\DirectXMath.h>
#include <GDK\Tools\commonresource.h>

#pragma warning(push)
#pragma warning(disable : 4005)
#include <stdint.h>
#pragma warning(pop)

#pragma pack(push,1)

const uint32_t GEOMETRY_RESOURCE_VERSION = 0x20315247; // "GR1 "

typedef enum GEOMETRY_RESOURCE_FORMAT
{
    GEOMETRY_RESOURCE_FORMAT_UNKNOWN                     = 0x00000000,
    GEOMETRY_RESOURCE_FORMAT_POSITION                    = 0x00000001,
    GEOMETRY_RESOURCE_FORMAT_NORMAL                      = 0x00000002,
    GEOMETRY_RESOURCE_FORMAT_TANGENT                     = 0x00000004,
    GEOMETRY_RESOURCE_FORMAT_BITANGENT                   = 0x00000008,
    GEOMETRY_RESOURCE_FORMAT_COLOR                       = 0x00000010,
    GEOMETRY_RESOURCE_FORMAT_TEXTURE0                    = 0x00000020,
    GEOMETRY_RESOURCE_FORMAT_TEXTURE1                    = 0x00000040,
    GEOMETRY_RESOURCE_FORMAT_TEXTURE2                    = 0x00000080,
    GEOMETRY_RESOURCE_FORMAT_TEXTURE3                    = 0x00000100,
    GEOMETRY_RESOURCE_FORMAT_BONEINDICES                 = 0x00000200,
    GEOMETRY_RESOURCE_FORMAT_BONEWEIGHTS                 = 0x00000400,
    GEOMETRY_RESOURCE_FORMAT_FORCE_UINT                  = 0xffffffff
} GEOMETRY_RESOURCE_FORMAT;

struct GEOMETRY_RESOURCE_VERTEX
{
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT3 Tangent;
    DirectX::XMFLOAT3 Bitangent;
    DirectX::XMFLOAT4 Color;
    DirectX::XMFLOAT2 TextureCoord0;
    DirectX::XMFLOAT2 TextureCoord1;
    DirectX::XMFLOAT2 TextureCoord2;
    DirectX::XMFLOAT2 TextureCoord3;
    DirectX::XMFLOAT4 BoneIndices;
    DirectX::XMFLOAT4 BoneWeights;
};

struct GEOMETRY_RESOURCE_FILEHEADER
{
    COMMON_RESOURCE_FILEHEADER Header;
    uint32_t Format;
    uint32_t VertexCount;
    uint32_t IndicesCount;
};

#pragma pack(pop)

[uuid("cc2e8c29-1c89-4fd4-b45d-099ea56fc8d5")]
struct IGeometryResource : public IUnknown
{
    virtual HRESULT Save(_In_ IStream* pStream) = 0;
    virtual HRESULT Load(_In_ IStream* pStream) = 0;
    
    virtual HRESULT Initialize(_In_ uint32_t format, _In_ GEOMETRY_RESOURCE_VERTEX* pVertices, _In_ size_t numVertices, _In_opt_ uint32_t* pIndices, _In_opt_ size_t numIndices) = 0;
    
    virtual HRESULT GetTotalVertices(_In_ size_t& totalVertices) = 0;
    virtual HRESULT GetTotalIndices(_In_ size_t& totalIndices) = 0;
    virtual HRESULT GetFormat(_In_ uint32_t& format) = 0;

    virtual HRESULT SetFormat(_In_ uint32_t format) = 0;
    virtual HRESULT SetColor(_In_ DirectX::XMFLOAT4 color) = 0;

    virtual HRESULT GenerateNormals() = 0;
    virtual HRESULT GenerateTangents() = 0;
    virtual HRESULT GenerateBitangents() = 0;

    virtual HRESULT GetVertices(GEOMETRY_RESOURCE_VERTEX** ppVertices, size_t& numVertices, uint32_t** ppIndices, size_t& numIndices) = 0;
    virtual HRESULT GetId(unsigned __int64& id) = 0;
};
