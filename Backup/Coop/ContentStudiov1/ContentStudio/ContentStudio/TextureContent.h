#pragma once
#include "BufferObject.h"
#include <D3D11.h>

#pragma pack(push,1)

struct TEXTURECONTENT_DDS_PIXELFORMAT
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwFourCC;
    DWORD dwRGBBitCount;
    DWORD dwRBitMask;
    DWORD dwGBitMask;
    DWORD dwBBitMask;
    DWORD dwABitMask;
};

struct TEXTURECONTENT_DDS_DX10HEADER
{
    DXGI_FORMAT dxgiFormat;
    D3D11_RESOURCE_DIMENSION resourceDimension;
    UINT miscFlag;
    UINT arraySize;
    UINT reserved;
};

struct TEXTURECONTENT_DDS_HEADER
{
    DWORD dwSize;
    DWORD dwHeaderFlags;
    DWORD dwHeight;
    DWORD dwWidth;
    DWORD dwPitchOrLinearSize;
    DWORD dwDepth; // only if DDS_HEADER_FLAGS_VOLUME is set in dwHeaderFlags
    DWORD dwMipMapCount;
    DWORD dwReserved1[11];
    TEXTURECONTENT_DDS_PIXELFORMAT ddspf;
    DWORD dwSurfaceFlags;
    DWORD dwCubemapFlags;
    DWORD dwReserved2[3];
    TEXTURECONTENT_DDS_DX10HEADER dx10Header;
};

struct TextureContentBufferDescription
{
    TEXTURECONTENT_DDS_HEADER header;
    CopiedBufferPtr pBuffer;
    std::wstring name;
};

#pragma pack(pop)

class ITextureContent
{
public:
    virtual HRESULT GetTextureName(std::wstring& name) = 0;
    virtual HRESULT SetTextureName(std::wstring& name) = 0;
    virtual HRESULT GetTextureBuffer(TextureContentBufferDescription& desc) = 0;
    virtual HRESULT SetTextureBuffer(TEXTURECONTENT_DDS_HEADER& header, void* pbuffer, DWORD dwSize) = 0;
    virtual HRESULT SaveToFile(LPCWSTR filePath) = 0;
    virtual HRESULT LoadFromFile(LPCWSTR filePath) = 0;
    virtual HRESULT LoadFromBuffer(size_t width, size_t height, DXGI_FORMAT format, size_t rowPitch, size_t slicePitch, BYTE* pixels) = 0;
    virtual HRESULT GenerateMipmaps(UINT count) = 0;
    virtual HRESULT ConvertToNormalMap(float amplitude) = 0;
    virtual HRESULT GetTextureStream(IStream** ppStream) = 0;
};

typedef std::shared_ptr<ITextureContent> ITextureContentPtr;