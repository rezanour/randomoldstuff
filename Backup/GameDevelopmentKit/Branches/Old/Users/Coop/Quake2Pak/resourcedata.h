#pragma once

#pragma pack(push,1)

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

struct FLOAT2
{
    float t;
    float u;
};

struct FLOAT3
{
    float x;
    float y;
    float z;
};

struct FLOAT4
{
    float x;
    float y;
    float z;
    float w;
};

struct GEOMETRY_RESOURCE_VERTEX
{
    FLOAT3 Position;
    FLOAT3 Normal;
    FLOAT3 Tangent;
    FLOAT3 Bitangent;
    FLOAT4 Color;
    FLOAT2 TextureCoord0;
    FLOAT2 TextureCoord1;
    FLOAT2 TextureCoord2;
    FLOAT2 TextureCoord3;
    FLOAT4 BoneIndices;
    FLOAT4 BoneWeights;
};

// IMPORTANT: This list should be trimmed down to reflect only the formats supported.
//            Currently it reflects ALL formats supported.
typedef enum TEXTURE_RESOURCE_FORMAT
{
    TEXTURE_RESOURCE_FORMAT_UNKNOWN                     = 0,
    TEXTURE_RESOURCE_FORMAT_R32G32B32A32_TYPELESS       = 1,
    TEXTURE_RESOURCE_FORMAT_R32G32B32A32_FLOAT          = 2,
    TEXTURE_RESOURCE_FORMAT_R32G32B32A32_UINT           = 3,
    TEXTURE_RESOURCE_FORMAT_R32G32B32A32_SINT           = 4,
    TEXTURE_RESOURCE_FORMAT_R32G32B32_TYPELESS          = 5,
    TEXTURE_RESOURCE_FORMAT_R32G32B32_FLOAT             = 6,
    TEXTURE_RESOURCE_FORMAT_R32G32B32_UINT              = 7,
    TEXTURE_RESOURCE_FORMAT_R32G32B32_SINT              = 8,
    TEXTURE_RESOURCE_FORMAT_R16G16B16A16_TYPELESS       = 9,
    TEXTURE_RESOURCE_FORMAT_R16G16B16A16_FLOAT          = 10,
    TEXTURE_RESOURCE_FORMAT_R16G16B16A16_UNORM          = 11,
    TEXTURE_RESOURCE_FORMAT_R16G16B16A16_UINT           = 12,
    TEXTURE_RESOURCE_FORMAT_R16G16B16A16_SNORM          = 13,
    TEXTURE_RESOURCE_FORMAT_R16G16B16A16_SINT           = 14,
    TEXTURE_RESOURCE_FORMAT_R32G32_TYPELESS             = 15,
    TEXTURE_RESOURCE_FORMAT_R32G32_FLOAT                = 16,
    TEXTURE_RESOURCE_FORMAT_R32G32_UINT                 = 17,
    TEXTURE_RESOURCE_FORMAT_R32G32_SINT                 = 18,
    TEXTURE_RESOURCE_FORMAT_R32G8X24_TYPELESS           = 19,
    TEXTURE_RESOURCE_FORMAT_D32_FLOAT_S8X24_UINT        = 20,
    TEXTURE_RESOURCE_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
    TEXTURE_RESOURCE_FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
    TEXTURE_RESOURCE_FORMAT_R10G10B10A2_TYPELESS        = 23,
    TEXTURE_RESOURCE_FORMAT_R10G10B10A2_UNORM           = 24,
    TEXTURE_RESOURCE_FORMAT_R10G10B10A2_UINT            = 25,
    TEXTURE_RESOURCE_FORMAT_R11G11B10_FLOAT             = 26,
    TEXTURE_RESOURCE_FORMAT_R8G8B8A8_TYPELESS           = 27,
    TEXTURE_RESOURCE_FORMAT_R8G8B8A8_UNORM              = 28,
    TEXTURE_RESOURCE_FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
    TEXTURE_RESOURCE_FORMAT_R8G8B8A8_UINT               = 30,
    TEXTURE_RESOURCE_FORMAT_R8G8B8A8_SNORM              = 31,
    TEXTURE_RESOURCE_FORMAT_R8G8B8A8_SINT               = 32,
    TEXTURE_RESOURCE_FORMAT_R16G16_TYPELESS             = 33,
    TEXTURE_RESOURCE_FORMAT_R16G16_FLOAT                = 34,
    TEXTURE_RESOURCE_FORMAT_R16G16_UNORM                = 35,
    TEXTURE_RESOURCE_FORMAT_R16G16_UINT                 = 36,
    TEXTURE_RESOURCE_FORMAT_R16G16_SNORM                = 37,
    TEXTURE_RESOURCE_FORMAT_R16G16_SINT                 = 38,
    TEXTURE_RESOURCE_FORMAT_R32_TYPELESS                = 39,
    TEXTURE_RESOURCE_FORMAT_D32_FLOAT                   = 40,
    TEXTURE_RESOURCE_FORMAT_R32_FLOAT                   = 41,
    TEXTURE_RESOURCE_FORMAT_R32_UINT                    = 42,
    TEXTURE_RESOURCE_FORMAT_R32_SINT                    = 43,
    TEXTURE_RESOURCE_FORMAT_R24G8_TYPELESS              = 44,
    TEXTURE_RESOURCE_FORMAT_D24_UNORM_S8_UINT           = 45,
    TEXTURE_RESOURCE_FORMAT_R24_UNORM_X8_TYPELESS       = 46,
    TEXTURE_RESOURCE_FORMAT_X24_TYPELESS_G8_UINT        = 47,
    TEXTURE_RESOURCE_FORMAT_R8G8_TYPELESS               = 48,
    TEXTURE_RESOURCE_FORMAT_R8G8_UNORM                  = 49,
    TEXTURE_RESOURCE_FORMAT_R8G8_UINT                   = 50,
    TEXTURE_RESOURCE_FORMAT_R8G8_SNORM                  = 51,
    TEXTURE_RESOURCE_FORMAT_R8G8_SINT                   = 52,
    TEXTURE_RESOURCE_FORMAT_R16_TYPELESS                = 53,
    TEXTURE_RESOURCE_FORMAT_R16_FLOAT                   = 54,
    TEXTURE_RESOURCE_FORMAT_D16_UNORM                   = 55,
    TEXTURE_RESOURCE_FORMAT_R16_UNORM                   = 56,
    TEXTURE_RESOURCE_FORMAT_R16_UINT                    = 57,
    TEXTURE_RESOURCE_FORMAT_R16_SNORM                   = 58,
    TEXTURE_RESOURCE_FORMAT_R16_SINT                    = 59,
    TEXTURE_RESOURCE_FORMAT_R8_TYPELESS                 = 60,
    TEXTURE_RESOURCE_FORMAT_R8_UNORM                    = 61,
    TEXTURE_RESOURCE_FORMAT_R8_UINT                     = 62,
    TEXTURE_RESOURCE_FORMAT_R8_SNORM                    = 63,
    TEXTURE_RESOURCE_FORMAT_R8_SINT                     = 64,
    TEXTURE_RESOURCE_FORMAT_A8_UNORM                    = 65,
    TEXTURE_RESOURCE_FORMAT_R1_UNORM                    = 66,
    TEXTURE_RESOURCE_FORMAT_R9G9B9E5_SHAREDEXP          = 67,
    TEXTURE_RESOURCE_FORMAT_R8G8_B8G8_UNORM             = 68,
    TEXTURE_RESOURCE_FORMAT_G8R8_G8B8_UNORM             = 69,
    TEXTURE_RESOURCE_FORMAT_BC1_TYPELESS                = 70,
    TEXTURE_RESOURCE_FORMAT_BC1_UNORM                   = 71,
    TEXTURE_RESOURCE_FORMAT_BC1_UNORM_SRGB              = 72,
    TEXTURE_RESOURCE_FORMAT_BC2_TYPELESS                = 73,
    TEXTURE_RESOURCE_FORMAT_BC2_UNORM                   = 74,
    TEXTURE_RESOURCE_FORMAT_BC2_UNORM_SRGB              = 75,
    TEXTURE_RESOURCE_FORMAT_BC3_TYPELESS                = 76,
    TEXTURE_RESOURCE_FORMAT_BC3_UNORM                   = 77,
    TEXTURE_RESOURCE_FORMAT_BC3_UNORM_SRGB              = 78,
    TEXTURE_RESOURCE_FORMAT_BC4_TYPELESS                = 79,
    TEXTURE_RESOURCE_FORMAT_BC4_UNORM                   = 80,
    TEXTURE_RESOURCE_FORMAT_BC4_SNORM                   = 81,
    TEXTURE_RESOURCE_FORMAT_BC5_TYPELESS                = 82,
    TEXTURE_RESOURCE_FORMAT_BC5_UNORM                   = 83,
    TEXTURE_RESOURCE_FORMAT_BC5_SNORM                   = 84,
    TEXTURE_RESOURCE_FORMAT_B5G6R5_UNORM                = 85,
    TEXTURE_RESOURCE_FORMAT_B5G5R5A1_UNORM              = 86,
    TEXTURE_RESOURCE_FORMAT_B8G8R8A8_UNORM              = 87,
    TEXTURE_RESOURCE_FORMAT_B8G8R8X8_UNORM              = 88,
    TEXTURE_RESOURCE_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
    TEXTURE_RESOURCE_FORMAT_B8G8R8A8_TYPELESS           = 90,
    TEXTURE_RESOURCE_FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
    TEXTURE_RESOURCE_FORMAT_B8G8R8X8_TYPELESS           = 92,
    TEXTURE_RESOURCE_FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
    TEXTURE_RESOURCE_FORMAT_BC6H_TYPELESS               = 94,
    TEXTURE_RESOURCE_FORMAT_BC6H_UF16                   = 95,
    TEXTURE_RESOURCE_FORMAT_BC6H_SF16                   = 96,
    TEXTURE_RESOURCE_FORMAT_BC7_TYPELESS                = 97,
    TEXTURE_RESOURCE_FORMAT_BC7_UNORM                   = 98,
    TEXTURE_RESOURCE_FORMAT_BC7_UNORM_SRGB              = 99,
    TEXTURE_RESOURCE_FORMAT_FORCE_UINT                  = 0xffffffff
} TEXTURE_RESOURCE_FORMAT;

struct COMMON_RESOURCE_FILEHEADER
{
    uint32_t         Version;  // set to XXXX_RESOURCE_VERSION
    char             Name[64]; // name of resource
    unsigned __int64 Id;
};

const uint32_t GEOMETRY_RESOURCE_VERSION = 0x20315247; // "GR1 "
    
struct GEOMETRY_RESOURCE_FILEHEADER
{
    COMMON_RESOURCE_FILEHEADER Header;
    uint32_t Format;
    uint32_t FrameCount;
};

struct GEOMETRY_RESOURCE_FRAME
{
    char name[64];
    uint32_t VertexCount;
    uint32_t IndicesCount;
};

// The Geometry Resource file is constructed as follows:

// [GEOMETRY_RESOURCE_FILEHEADER ]  contains version, metadata
// [GEOMETRY_RESOURCE_FRAME      ]  contains individual frame
// [GEOMETRY_RESOURCE_FRAME      ]            .
// [GEOMETRY_RESOURCE_FRAME      ]            .

const uint32_t TEXTURE_RESOURCE_VERSION = 0x20315254; // "TR1 "
    
struct TEXTURE_RESOURCE_IMAGE
{
    size_t      width;
    size_t      height;
    TEXTURE_RESOURCE_FORMAT format;
    size_t      rowPitch;
    size_t      slicePitch;
    uint32_t    cbPixels;
    uint8_t*    pixels;
};

enum TEXTURE_RESOURCE_DIMENSION
{
    TEXTURE_RESOURCE_DIMENSION_UNKNOWN      = 0,
    TEXTURE_RESOURCE_DIMENSION_TEXTURE1D    = 2,
    TEXTURE_RESOURCE_DIMENSION_TEXTURE2D    = 3,
    TEXTURE_RESOURCE_DIMENSION_TEXTURE3D    = 4,
};

enum TEXTURE_RESOURCE_MISC_FLAG
{
    TEXTURE_RESOURCE_MISC_TEXTURECUBE = 0x4L,
};

struct TEXTURE_RESOURCE_METADATA
{
    size_t          width;
    size_t          height;     // Should be 1 for 1D textures
    size_t          depth;      // Should be 1 for 1D or 2D textures
    size_t          arraySize;  // For cubemap, this is a multiple of 6
    size_t          mipLevels;
    uint32_t        miscFlags;
    TEXTURE_RESOURCE_FORMAT     format;
    TEXTURE_RESOURCE_DIMENSION dimension;
};

struct TEXTURE_RESOURCE_FILEHEADER
{
    COMMON_RESOURCE_FILEHEADER header;
    TEXTURE_RESOURCE_METADATA metadata;
};

// The Texture Resource file is constructed as follows:

// [TEXTURE_RESOURCE_FILEHEADER ]  contains version, metadata
// [TEXTURE_RESOURCE_IMAGE      ]  contains individual image
// [TEXTURE_RESOURCE_IMAGE      ]            .
// [TEXTURE_RESOURCE_IMAGE      ]            .
// [TEXTURE_RESOURCE_IMAGE      ]            .

#pragma pack(pop)
