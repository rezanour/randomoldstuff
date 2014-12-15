#pragma once

struct ImageMetadata
{
    uint32_t    Width;
    uint32_t    Height;
    uint32_t    MipLevels;
    uint32_t    ArraySize;
    DXGI_FORMAT Format;
};

void DDSGetImageMetadata(_In_z_ const char* filename, _Out_ ImageMetadata* metadata);
void DDSLoadMipsIntoResource(_In_z_ const char* filename, uint32_t startMip, uint32_t numMips, _In_ ID3D11Texture2D* dest, uint32_t destIndex, uint32_t destStartMip);
