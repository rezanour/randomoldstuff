#pragma once

namespace GDK {
    struct IString;
namespace Content {

    struct IPersistResource;
    struct ITextureResource;
    struct ITextureResourceEdit;
    struct IGeometryResource;
    struct IGeometryResourceEdit;
    struct IWorldResource;
    struct IWorldResourceEdit;
    
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

    const size_t MAX_TYPE_LENGTH = 128;
    const size_t MAX_ID_LENGTH = 64;

    struct WORLD_RESOURCE_OBJECT
    {
        wchar_t Type[MAX_TYPE_LENGTH];
        wchar_t Id[MAX_ID_LENGTH];
        FLOAT3 Position;
        float Rotation;
    };

#ifdef WIN32
    GDKINTERFACE IPersistResource : public IUnknown
    {
        IMPL_GUID(0xc0b6bfe1, 0xa8df, 0x4b91, 0x83, 0xef, 0x68, 0x2c, 0xfb, 0xbd, 0xc9, 0x2c);

        virtual HRESULT GDKAPI Save(_In_ GDK::IStream* output) = 0;
        virtual HRESULT GDKAPI Load(_In_ GDK::IStream* input) = 0;
        virtual HRESULT GDKAPI SetName(_In_ const wchar_t* name) = 0;
    };

    GDKINTERFACE ITextureResourceEdit : public IPersistResource
    {
        IMPL_GUID(0xbb90f93b, 0x26eb, 0x48b2, 0xba, 0x47, 0x9b, 0x2c, 0xc1, 0xbe, 0x63, 0x21);

        virtual HRESULT GDKAPI CreateTextureResource(_COM_Outptr_ ITextureResource** resource) = 0;
    };
   
    GDKINTERFACE IGeometryResourceEdit : public IPersistResource
    {
        IMPL_GUID(0x838a489e, 0x66e7, 0x4889, 0xbf, 0x15, 0x34, 0x4d, 0x60, 0x8a, 0xa5, 0xb7);

        virtual HRESULT GDKAPI CreateGeometryResource(_COM_Outptr_ IGeometryResource** resource) = 0;
    };

    GDKINTERFACE IWorldResourceEdit : public IPersistResource
    {
        IMPL_GUID(0xa484f5fa, 0x2a24, 0x489f, 0xae, 0xc9, 0x4c, 0x83, 0x93, 0xe, 0x96, 0xcb);

        virtual HRESULT GDKAPI CreateWorldResource(_COM_Outptr_ IWorldResource** resource) = 0;
    };

    GDKINTERFACE IResourceFactory : public IUnknown
    {
        IMPL_GUID(0xcc1d8b26, 0xb433, 0x4169, 0x82, 0x4e, 0x34, 0x30, 0x53, 0xe4, 0xd3, 0x20);

        virtual HRESULT GDKAPI CreateTextureResource(_COM_Outptr_ ITextureResourceEdit** resource) = 0;
        virtual HRESULT GDKAPI CreateGeometryResource(_COM_Outptr_ IGeometryResourceEdit** resource) = 0;
        virtual HRESULT GDKAPI CreateWorldResource(_COM_Outptr_ IWorldResourceEdit** resource) = 0;
    };
#endif

    GDKINTERFACE ITextureResource : public IUnknown
    {
        IMPL_GUID(0xaa9c9b9, 0x8d98, 0x4d91, 0xb0, 0x98, 0xf5, 0xf4, 0xc2, 0x9d, 0x8d, 0x68);

        virtual HRESULT GDKAPI GetFormat(_Out_ uint32_t* format) = 0;
        virtual HRESULT GDKAPI GetImageCount(_Out_ uint32_t* count) = 0;
        virtual HRESULT GDKAPI GetSize(_In_ uint32_t index, _Out_ uint32_t* width, uint32_t* height) = 0;
        virtual HRESULT GDKAPI GetPixels(_In_ uint32_t index, _Out_ const byte_t** pixels) = 0;
    };

    GDKINTERFACE IGeometryResource : public IUnknown
    {
        IMPL_GUID(0x5328299, 0xcdf9, 0x4176, 0x81, 0x7e, 0xbb, 0x57, 0x5c, 0x6d, 0xc8, 0x6d);

        virtual HRESULT GDKAPI GetName(_Out_ const wchar_t** name) = 0;
        virtual HRESULT GDKAPI GetFormat(_Out_ uint32_t* format) = 0;
        virtual HRESULT GDKAPI GetVertices(_Out_ uint32_t* count, _Out_ const GEOMETRY_RESOURCE_VERTEX** vertices) = 0;
        virtual HRESULT GDKAPI GetIndices(_Out_ uint32_t* count, _Out_ const uint32_t** indices) = 0;

        virtual HRESULT GDKAPI GetFrameCount(_Out_ uint32_t* count) = 0;
        virtual HRESULT GDKAPI GetFrameName(_In_ uint32_t frame, _Out_ const wchar_t** name) = 0;
        virtual HRESULT GDKAPI GetFrameVertices(_In_ uint32_t frame, _Out_ uint32_t* count, const _Out_ GEOMETRY_RESOURCE_VERTEX** vertices) = 0;
        virtual HRESULT GDKAPI GetFrameIndices(_In_ uint32_t frame, _Out_ uint32_t* count, _Out_ const uint32_t** indices) = 0;
    };

    GDKINTERFACE IWorldResource : public IUnknown
    {
        IMPL_GUID(0xbe8f5ebc, 0xa2c2, 0x41a8, 0xb8, 0x59, 0xd, 0x3f, 0x50, 0xe3, 0xef, 0x94);

        virtual HRESULT GDKAPI GetName(_Out_ const wchar_t** name) = 0;
        virtual HRESULT GDKAPI GetWorldName(_Out_ const wchar_t** name) = 0;
        virtual HRESULT GDKAPI GetObjects(_Out_ uint32_t* count, _Out_ const WORLD_RESOURCE_OBJECT** objects) = 0;
    };
} // Content
} // GDK
