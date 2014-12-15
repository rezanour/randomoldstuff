#pragma once

#include <Windows.h>
#include <GDK\Tools\commonresource.h>

#pragma warning(push)
#pragma warning(disable : 4005)
#include <stdint.h>
#pragma warning(pop)

#pragma pack(push,1)

const uint32_t TEXTURE_RESOURCE_VERSION = 0x20315254; // "TR1 "

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

enum TextureResourceSaveFormat
{
    textureresource,
    bmp,
    png,
    jpg,
    dds,
    dds9,
    tga
};

[uuid("0abdf10b-b704-479b-bf8f-0aaa328d4121")]
struct ITextureResource : public IUnknown
{
    virtual HRESULT Save(_In_ IStream* pStream, _In_ TextureResourceSaveFormat outputFormat) = 0;
    virtual HRESULT Load(_In_ IStream* pStream) = 0;
    virtual HRESULT Load(_In_ LPCWSTR filePath) = 0;

    virtual HRESULT Initialize( _In_ TEXTURE_RESOURCE_METADATA* pMetadata ) = 0;

    virtual HRESULT Initialize1D( _In_ TEXTURE_RESOURCE_FORMAT fmt, _In_ size_t length, _In_ size_t arraySize, _In_ size_t mipLevels ) = 0;
    virtual HRESULT Initialize2D( _In_ TEXTURE_RESOURCE_FORMAT fmt, _In_ size_t width, _In_ size_t height, _In_ size_t arraySize, _In_ size_t mipLevels ) = 0;
    virtual HRESULT Initialize3D( _In_ TEXTURE_RESOURCE_FORMAT fmt, _In_ size_t width, _In_ size_t height, _In_ size_t depth, _In_ size_t mipLevels ) = 0;
    virtual HRESULT InitializeCube( _In_ TEXTURE_RESOURCE_FORMAT fmt, _In_ size_t width, _In_ size_t height, _In_ size_t nCubes, _In_ size_t mipLevels ) = 0;

    virtual HRESULT InitializeFromImage( _In_ TEXTURE_RESOURCE_IMAGE* pImage, _In_ bool allow1D) = 0;
    virtual HRESULT InitializeArrayFromImages( _In_count_(nImages) TEXTURE_RESOURCE_IMAGE* images, _In_ size_t nImages, _In_ bool allow1D) = 0;
    virtual HRESULT InitializeCubeFromImages( _In_count_(nImages) TEXTURE_RESOURCE_IMAGE* images, _In_ size_t nImages ) = 0;
    virtual HRESULT Initialize3DFromImages( _In_count_(depth) TEXTURE_RESOURCE_IMAGE* images, _In_ size_t depth ) = 0;

    virtual HRESULT GetMetadata( _In_ TEXTURE_RESOURCE_METADATA* pMetadata ) = 0;
    virtual HRESULT GetImage(_In_ size_t mip, _In_ size_t item, _In_ size_t slice, TEXTURE_RESOURCE_IMAGE** ppImage) = 0;
    virtual HRESULT GetPixels(uint8_t** ppPixels, size_t* ppixelsSize) = 0;
    virtual HRESULT GetImageCount(size_t* pnumImages) = 0;
    virtual HRESULT SetName(LPCSTR name) = 0;
    virtual HRESULT GetId(unsigned __int64& id) = 0;
};
