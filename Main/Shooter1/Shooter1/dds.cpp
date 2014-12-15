#include "precomp.h"
#include "dds.h"

#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) |       \
                ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

#define DDS_FOURCC      0x00000004  // DDPF_FOURCC
#define DDS_RGB         0x00000040  // DDPF_RGB
#define DDS_LUMINANCE   0x00020000  // DDPF_LUMINANCE
#define DDS_ALPHA       0x00000002  // DDPF_ALPHA

#pragma pack(push,1)

const uint32_t DDS_MAGIC = 0x20534444; // "DDS "

struct DDS_PIXELFORMAT
{
    uint32_t    size;
    uint32_t    flags;
    uint32_t    fourCC;
    uint32_t    RGBBitCount;
    uint32_t    RBitMask;
    uint32_t    GBitMask;
    uint32_t    BBitMask;
    uint32_t    ABitMask;
};

struct DDS_HEADER
{
    uint32_t        size;
    uint32_t        flags;
    uint32_t        height;
    uint32_t        width;
    uint32_t        pitchOrLinearSize;
    uint32_t        depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
    uint32_t        mipMapCount;
    uint32_t        reserved1[11];
    DDS_PIXELFORMAT ddspf;
    uint32_t        caps;
    uint32_t        caps2;
    uint32_t        caps3;
    uint32_t        caps4;
    uint32_t        reserved2;
};

struct DDS_HEADER_DXT10
{
    DXGI_FORMAT     dxgiFormat;
    uint32_t        resourceDimension;
    uint32_t        miscFlag; // see D3D11_RESOURCE_MISC_FLAG
    uint32_t        arraySize;
    uint32_t        miscFlags2;
};

#pragma pack(pop)

static uint32_t BitsPerPixel(DXGI_FORMAT fmt)
{
    switch( fmt )
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 96;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return 64;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return 32;

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:

#ifdef DXGI_1_2_FORMATS
    case DXGI_FORMAT_B4G4R4A4_UNORM:
#endif
        return 16;

    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
        return 8;

    case DXGI_FORMAT_R1_UNORM:
        return 1;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return 4;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return 8;

    default:
        return 0;
    }
}

static void GetSurfaceInfo( _In_ uint32_t width,
                            _In_ uint32_t height,
                            _In_ DXGI_FORMAT fmt,
                            _Out_opt_ uint32_t* outNumBytes,
                            _Out_opt_ uint32_t* outRowBytes,
                            _Out_opt_ uint32_t* outNumRows )
{
    uint32_t numBytes = 0;
    uint32_t rowBytes = 0;
    uint32_t numRows = 0;

    bool bc = false;
    bool packed  = false;
    uint32_t bcnumBytesPerBlock = 0;
    switch (fmt)
    {
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        bc=true;
        bcnumBytesPerBlock = 8;
        break;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        bc = true;
        bcnumBytesPerBlock = 16;
        break;

    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        packed = true;
        break;
    }

    if (bc)
    {
        uint32_t numBlocksWide = 0;
        if (width > 0)
        {
            numBlocksWide = std::max<uint32_t>( 1, (width + 3) / 4 );
        }
        uint32_t numBlocksHigh = 0;
        if (height > 0)
        {
            numBlocksHigh = std::max<uint32_t>( 1, (height + 3) / 4 );
        }
        rowBytes = numBlocksWide * bcnumBytesPerBlock;
        numRows = numBlocksHigh;
    }
    else if (packed)
    {
        rowBytes = ( ( width + 1 ) >> 1 ) * 4;
        numRows = height;
    }
    else
    {
        uint32_t bpp = BitsPerPixel( fmt );
        rowBytes = ( width * bpp + 7 ) / 8; // round up to nearest byte
        numRows = height;
    }

    numBytes = rowBytes * numRows;
    if (outNumBytes)
    {
        *outNumBytes = numBytes;
    }
    if (outRowBytes)
    {
        *outRowBytes = rowBytes;
    }
    if (outNumRows)
    {
        *outNumRows = numRows;
    }
}

#define ISBITMASK( r,g,b,a ) ( ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a )

static DXGI_FORMAT GetDXGIFormat(const DDS_PIXELFORMAT& ddpf)
{
    if (ddpf.flags & DDS_RGB)
    {
        // Note that sRGB formats are written using the "DX10" extended header

        switch (ddpf.RGBBitCount)
        {
        case 32:
            if (ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0xff000000))
            {
                return DXGI_FORMAT_R8G8B8A8_UNORM;
            }

            if (ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0xff000000))
            {
                return DXGI_FORMAT_B8G8R8A8_UNORM;
            }

            if (ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0x00000000))
            {
                return DXGI_FORMAT_B8G8R8X8_UNORM;
            }

            if (ISBITMASK(0x0000ffff,0xffff0000,0x00000000,0x00000000))
            {
                return DXGI_FORMAT_R16G16_UNORM;
            }

            if (ISBITMASK(0xffffffff,0x00000000,0x00000000,0x00000000))
            {
                // Only 32-bit color channel format in D3D9 was R32F
                return DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114
            }
            break;

        case 16:
            if (ISBITMASK(0x7c00,0x03e0,0x001f,0x8000))
            {
                return DXGI_FORMAT_B5G5R5A1_UNORM;
            }
            if (ISBITMASK(0xf800,0x07e0,0x001f,0x0000))
            {
                return DXGI_FORMAT_B5G6R5_UNORM;
            }

            // No DXGI format maps to ISBITMASK(0x7c00,0x03e0,0x001f,0x0000) aka D3DFMT_X1R5G5B5

#ifdef DXGI_1_2_FORMATS
            if (ISBITMASK(0x0f00,0x00f0,0x000f,0xf000))
            {
                return DXGI_FORMAT_B4G4R4A4_UNORM;
            }

            // No DXGI format maps to ISBITMASK(0x0f00,0x00f0,0x000f,0x0000) aka D3DFMT_X4R4G4B4
#endif

            // No 3:3:2, 3:3:2:8, or paletted DXGI formats aka D3DFMT_A8R3G3B2, D3DFMT_R3G3B2, D3DFMT_P8, D3DFMT_A8P8, etc.
            break;
        }
    }
    else if (ddpf.flags & DDS_LUMINANCE)
    {
        if (8 == ddpf.RGBBitCount)
        {
            if (ISBITMASK(0x000000ff,0x00000000,0x00000000,0x00000000))
            {
                return DXGI_FORMAT_R8_UNORM; // D3DX10/11 writes this out as DX10 extension
            }

            // No DXGI format maps to ISBITMASK(0x0f,0x00,0x00,0xf0) aka D3DFMT_A4L4
        }

        if (16 == ddpf.RGBBitCount)
        {
            if (ISBITMASK(0x0000ffff,0x00000000,0x00000000,0x00000000))
            {
                return DXGI_FORMAT_R16_UNORM; // D3DX10/11 writes this out as DX10 extension
            }
            if (ISBITMASK(0x000000ff,0x00000000,0x00000000,0x0000ff00))
            {
                return DXGI_FORMAT_R8G8_UNORM; // D3DX10/11 writes this out as DX10 extension
            }
        }
    }
    else if (ddpf.flags & DDS_ALPHA)
    {
        if (8 == ddpf.RGBBitCount)
        {
            return DXGI_FORMAT_A8_UNORM;
        }
    }
    else if (ddpf.flags & DDS_FOURCC)
    {
        if (MAKEFOURCC( 'D', 'X', 'T', '1' ) == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC1_UNORM;
        }
        if (MAKEFOURCC( 'D', 'X', 'T', '3' ) == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC2_UNORM;
        }
        if (MAKEFOURCC( 'D', 'X', 'T', '5' ) == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC3_UNORM;
        }

        // While pre-mulitplied alpha isn't directly supported by the DXGI formats,
        // they are basically the same as these BC formats so they can be mapped
        if (MAKEFOURCC( 'D', 'X', 'T', '2' ) == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC2_UNORM;
        }
        if (MAKEFOURCC( 'D', 'X', 'T', '4' ) == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC3_UNORM;
        }

        if (MAKEFOURCC( 'A', 'T', 'I', '1' ) == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC4_UNORM;
        }
        if (MAKEFOURCC( 'B', 'C', '4', 'U' ) == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC4_UNORM;
        }
        if (MAKEFOURCC( 'B', 'C', '4', 'S' ) == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC4_SNORM;
        }

        if (MAKEFOURCC( 'A', 'T', 'I', '2' ) == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC5_UNORM;
        }
        if (MAKEFOURCC( 'B', 'C', '5', 'U' ) == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC5_UNORM;
        }
        if (MAKEFOURCC( 'B', 'C', '5', 'S' ) == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC5_SNORM;
        }

        // BC6H and BC7 are written using the "DX10" extended header

        if (MAKEFOURCC( 'R', 'G', 'B', 'G' ) == ddpf.fourCC)
        {
            return DXGI_FORMAT_R8G8_B8G8_UNORM;
        }
        if (MAKEFOURCC( 'G', 'R', 'G', 'B' ) == ddpf.fourCC)
        {
            return DXGI_FORMAT_G8R8_G8B8_UNORM;
        }

        // Check for D3DFORMAT enums being set here
        switch( ddpf.fourCC )
        {
        case 36: // D3DFMT_A16B16G16R16
            return DXGI_FORMAT_R16G16B16A16_UNORM;

        case 110: // D3DFMT_Q16W16V16U16
            return DXGI_FORMAT_R16G16B16A16_SNORM;

        case 111: // D3DFMT_R16F
            return DXGI_FORMAT_R16_FLOAT;

        case 112: // D3DFMT_G16R16F
            return DXGI_FORMAT_R16G16_FLOAT;

        case 113: // D3DFMT_A16B16G16R16F
            return DXGI_FORMAT_R16G16B16A16_FLOAT;

        case 114: // D3DFMT_R32F
            return DXGI_FORMAT_R32_FLOAT;

        case 115: // D3DFMT_G32R32F
            return DXGI_FORMAT_R32G32_FLOAT;

        case 116: // D3DFMT_A32B32G32R32F
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
    }

    return DXGI_FORMAT_UNKNOWN;
}

// File pointer will be just after the headers, at the beginning of data.
static FileHandle OpenDDSFile(_In_z_ const char* filename, _Out_ uint32_t* fileSize, _Out_ DDS_HEADER* header, _Out_ bool* hasDx10Header, _Out_ DDS_HEADER_DXT10* dx10Header, _Out_ uint32_t* dataSize)
{
    assert(filename != nullptr);

    FileHandle file(CreateFile(filename, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    CHECKGLE(file.IsValid());

    FILE_STANDARD_INFO fileInfo;
    CHECKGLE(GetFileInformationByHandleEx(file.Get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)));

    if (fileInfo.EndOfFile.HighPart > 0)
    {
        Error("File too large.");
    }

    if (fileInfo.EndOfFile.LowPart < sizeof(DDS_HEADER) + sizeof(uint32_t))
    {
        Error("File too small to be a DDS. No header.");
    }

    *fileSize = fileInfo.EndOfFile.LowPart;

    DWORD bytesRead;
    uint32_t magicNumber;

    CHECKGLE(ReadFile(file.Get(), &magicNumber, sizeof(magicNumber), &bytesRead, nullptr));
    CHECKTRUE(bytesRead == sizeof(magicNumber));

    CHECKTRUE(magicNumber == DDS_MAGIC);

    CHECKGLE(ReadFile(file.Get(), header, sizeof(DDS_HEADER), &bytesRead, nullptr));
    CHECKTRUE(bytesRead == sizeof(DDS_HEADER));

    // Validate header
    CHECKTRUE(header->size == sizeof(DDS_HEADER));
    CHECKTRUE(header->ddspf.size == sizeof(DDS_PIXELFORMAT));

    // Check for DX10 extension
    *hasDx10Header = false;
    if ((header->ddspf.flags & DDS_FOURCC) &&
        (header->ddspf.fourCC == MAKEFOURCC( 'D', 'X', '1', '0' )))
    {
        // Must be long enough for both headers and magic value
        CHECKTRUE((*fileSize) > (sizeof(DDS_HEADER) + sizeof(uint32_t) + sizeof(DDS_HEADER_DXT10)));

        *hasDx10Header = true;

        CHECKTRUE(ReadFile(file.Get(), dx10Header, sizeof(DDS_HEADER_DXT10), &bytesRead, nullptr));
        CHECKTRUE(bytesRead == sizeof(DDS_HEADER_DXT10));
    }

    *dataSize = (*fileSize - (sizeof(uint32_t) + sizeof(DDS_HEADER) + (*hasDx10Header ? sizeof(DDS_HEADER_DXT10) : 0)));

    return file;
}

_Use_decl_annotations_
void DDSGetImageMetadata(const char* filename, ImageMetadata* metadata)
{
    uint32_t fileSize, dataSize;
    bool hasDx10Header;
    DDS_HEADER header;
    DDS_HEADER_DXT10 dx10Header;

    FileHandle file(OpenDDSFile(filename, &fileSize, &header, &hasDx10Header, &dx10Header, &dataSize));
    // Guaranteed to be valid if the function didn't throw

    metadata->Width = header.width;
    metadata->Height = header.height;
    metadata->MipLevels = header.mipMapCount;
    metadata->Format = hasDx10Header ? dx10Header.dxgiFormat : GetDXGIFormat(header.ddspf);
    metadata->ArraySize = hasDx10Header ? dx10Header.arraySize : 1;
}

_Use_decl_annotations_
void DDSLoadMipsIntoResource(const char* filename, uint32_t startMip, uint32_t numMips, ID3D11Texture2D* dest, uint32_t destIndex, uint32_t destStartMip)
{
    D3D11_TEXTURE2D_DESC desc;
    dest->GetDesc(&desc);

    assert(destStartMip + numMips <= desc.MipLevels);

    uint32_t fileSize, dataSize;
    bool hasDx10Header;
    DDS_HEADER header;
    DDS_HEADER_DXT10 dx10Header;

    FileHandle file(OpenDDSFile(filename, &fileSize, &header, &hasDx10Header, &dx10Header, &dataSize));
    // Guaranteed to be valid if the function didn't throw

    assert(startMip + numMips <= header.mipMapCount);

    ComPtr<ID3D11Device> device;
    dest->GetDevice(&device);

    ComPtr<ID3D11DeviceContext> context;
    device->GetImmediateContext(&context);

    DXGI_FORMAT format = hasDx10Header ? dx10Header.dxgiFormat : GetDXGIFormat(header.ddspf);

    uint32_t numBytes, rowBytes, numRows;
    uint32_t width = header.width;
    uint32_t height = header.height;

    // Advance to starting source mip
    uint32_t totalSizeToSkip = 0;
    for (uint32_t i = 0; i < startMip; ++i)
    {
        GetSurfaceInfo(width, height, format, &numBytes, &rowBytes, &numRows);
        width /= 2;
        height /= 2;

        totalSizeToSkip += numBytes;
    }

    CHECKTRUE(SetFilePointer(file.Get(), totalSizeToSkip, nullptr, FILE_CURRENT) != INVALID_SET_FILE_POINTER);

    DWORD bytesRead;
    for (uint32_t i = 0; i < numMips; ++i)
    {
        UINT subResource = D3D11CalcSubresource(destStartMip + i, destIndex, desc.MipLevels);

        GetSurfaceInfo(width, height, format, &numBytes, &rowBytes, &numRows);

        D3D11_MAPPED_SUBRESOURCE mapped;
        CHECKHR(context->Map(dest, subResource, D3D11_MAP_WRITE, 0, &mapped));

        CHECKTRUE(mapped.DepthPitch / mapped.RowPitch == height);

        for (uint32_t row = 0; row < height; ++row)
        {
            CHECKTRUE(ReadFile(file.Get(), (void*)((uint8_t*)mapped.pData + (row * mapped.RowPitch)), rowBytes, &bytesRead, nullptr));
            CHECKTRUE(bytesRead == rowBytes);
        }

        context->Unmap(dest, subResource);

        width /= 2;
        height /= 2;
    }
}
