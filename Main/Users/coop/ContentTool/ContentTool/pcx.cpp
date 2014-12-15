#include "stdafx.h"

bool ConvertPCXTo8bitBMP(_In_ std::vector<uint8_t>& pcx, _Inout_ std::vector<uint8_t>& bmp)
{
    bmp.clear();

    // PCX
    pcx_fileheader_t* pcxHeader = (pcx_fileheader_t*)pcx.data();
    uint8_t* data = pcx.data();

    // This code only supports 8-bit PCX
    if (pcxHeader->bpp != 8)
    {
        return false;
    }

    // BMP
    bmp_infoheader_t bmih = {0};
    bmp_paletteentry_t bmpal[256];
    std::vector<uint8_t> bmpData;

    memset(bmpal, 0, sizeof(bmpal));

    int32_t scanlineSize = 0;
    std::vector<uint8_t> scanLineTemp;

    scanlineSize = pcxHeader->planes * pcxHeader->bpl;

    //ldiv_t divResult = ldiv(scanlineSize, sizeof(int16_t));
    //if (divResult.rem > 0)
    //{
    //    scanlineSize = (scanlineSize/sizeof(int16_t) + 1) * sizeof(int16_t);
    //}
    
    bmp_fileheader_t bmfh;

    // bitmap file header
    bmfh.size      = sizeof(bmp_fileheader_t);
    bmfh.type      = BitmapFileType;
    bmfh.reserved1 = 0;
    bmfh.reserved2 = 0;
    bmfh.offBits   = bmfh.size + sizeof(bmih) + sizeof(bmpal);

    // bitmap info header
    bmih.size           = sizeof(bmp_infoheader_t);
    bmih.width          = pcxHeader->x2 - pcxHeader->x1 + 1;
    bmih.height         = pcxHeader->y2 - pcxHeader->y1 + 1;
    bmih.planes         = pcxHeader->planes;
    bmih.bitCount       = pcxHeader->bpp;
    bmih.compression    = 0;
    bmih.sizeImage      = scanlineSize * bmih.height;
    bmih.xPelsPerMeter  = 0;
    bmih.yPelsPerMeter  = 0;
    bmih.clrUsed        = 0;
    bmih.clrImportant   = 0;

    int32_t lDataPos = 0;
    int32_t lPos = 128;
    int32_t iX = 0;

    // bitmap data
    bmpData.resize(bmih.sizeImage);
    for ( int32_t iY = 0; iY < bmih.height; iY++ )
    {
        // Decompress the scan line
        for ( iX = 0; iX < pcxHeader->bpl; )
        {
            uint8_t uiValue = data[lPos++];
            if ( uiValue > 192 ) 
            {
                uiValue -= 192;  // Repeat count
                uint8_t pixelColor = data[lPos++];

                if ( iX <= bmih.width )
                {
                    for ( uint8_t bRepeat = 0; bRepeat < uiValue; bRepeat++ )
                    {
                        bmpData[lDataPos++] = pixelColor;
                        iX++;
                    }
                }
                else
                {
                    iX += uiValue; // skip
                }
            }
            else
            {
                if ( iX <= bmih.width )
                {
                    bmpData[lDataPos++] = uiValue;
                }
                iX++;
            }
        }

        // Pad the rest with zeros
        if ( iX < scanlineSize )
        {
            for ( ;iX < scanlineSize; iX++ )
            {
                bmpData[lDataPos++] = 0;
            }
        }
    }

    // bitmap palette
    if ( data[lPos++] == 12 )
    {
        for ( int16_t Entry=0; Entry < _countof(bmpal); Entry++ )
        {
            bmpal[Entry].red      = data[lPos++];
            bmpal[Entry].green    = data[lPos++];
            bmpal[Entry].blue     = data[lPos++];
            bmpal[Entry].reserved = 0;
        }
    }

    // contruct a full bmp file in memory as the output
    bmp.resize(bmfh.offBits + bmih.sizeImage);

    uint8_t* copyData = bmp.data();
    memset(copyData, 0, bmp.size());
    memcpy(copyData, (void*)&bmfh, sizeof(bmfh)); copyData += sizeof(bmfh);
    memcpy(copyData, (void*)&bmih, sizeof(bmih)); copyData += sizeof(bmih);
    memcpy(copyData, (void*)bmpal, sizeof(bmpal)); copyData += sizeof(bmpal);
    memcpy(copyData, (void*)bmpData.data(), bmpData.size());

    return true;
}

bool GetPaletteFromPCX(_In_ std::vector<uint8_t>& pcx, _Inout_ std::vector<bmp_paletteentry_t>& pal)
{
    bool success = true;
    std::vector<uint8_t> bmp;
    success = ConvertPCXTo8bitBMP(pcx, bmp);
    if (success)
    {
        bmp_paletteentry_t* bmpal = (bmp_paletteentry_t*)(bmp.data() + (sizeof(bmp_fileheader_t) + sizeof(bmp_infoheader_t)));
        pal.resize(256);
        memcpy(pal.data(), bmpal, (sizeof(bmp_paletteentry_t) * pal.size()));
    }

    return success;
}

bool ConvertPCXTo32bitBMP(_In_ std::vector<uint8_t>& pcx, _Inout_ std::vector<uint8_t>& bmp)
{
    bool success = true;
    bmp.clear();
    std::vector<uint8_t> bmp8bit;
    bmp_fileheader_t* bmfh = nullptr;
    bmp_infoheader_t* bmih = nullptr;
    bmp_paletteentry_t* bmpal = nullptr;
    uint8_t* bmpbits = nullptr;
    uint32_t bmp8BitImageSize = 0;

    success = ConvertPCXTo8bitBMP(pcx, bmp8bit);
    if (success)
    {
        bmfh = (bmp_fileheader_t*)bmp8bit.data();
        bmih = (bmp_infoheader_t*)(bmp8bit.data() + sizeof(bmp_fileheader_t));
        bmpal = (bmp_paletteentry_t*)(bmp8bit.data() + sizeof(bmp_fileheader_t) + sizeof(bmp_infoheader_t));
        bmpbits = (uint8_t*)(bmp8bit.data() + bmfh->offBits);

        bmp8BitImageSize = bmih->sizeImage;

        bmfh->offBits = bmfh->size + sizeof(bmp_infoheader_t);
        bmih->bitCount = 32;
        bmih->sizeImage = bmih->sizeImage * 4;

        // contruct a full bmp file in memory as the output
        bmp.resize(bmfh->offBits + bmih->sizeImage);
        uint8_t* copyData = bmp.data();

        memcpy(copyData, bmfh, sizeof(bmp_fileheader_t)); copyData += sizeof(bmp_fileheader_t);
        memcpy(copyData, bmih, sizeof(bmp_infoheader_t)); copyData += sizeof(bmp_infoheader_t);

        for (uint32_t i = 0; i < bmp8BitImageSize;i++)
        {
            bmp_paletteentry_t palEntry;
            memset(&palEntry, 0, sizeof(palEntry));

            // Look for Quake II transparent color
            if (bmpal[bmpbits[i]].red == 159 && bmpal[bmpbits[i]].green == 91 && bmpal[bmpbits[i]].blue == 83)
            {
                // Transparent pixel detected
            }
            else
            {
                // Opaque pixel detected
                palEntry = bmpal[bmpbits[i]];
                palEntry.reserved = 255;
            }

            memcpy(copyData, &palEntry, sizeof(palEntry));
            copyData += sizeof(palEntry);
        }
    }

    return success;
}

void GetCommonPalette(_In_ std::ifstream& inputfile, _In_ std::vector<pak_entry_t>& entries, _Inout_ std::vector<bmp_paletteentry_t>& pal)
{
    std::vector<uint8_t> pcx;
    pak_entry_t commonPaletteEntry = {};
    for (size_t i = 0; i < entries.size(); i++)
    {
        std::string entryName = GetPakEntryName(entries[i]);
        if (entryName == "pics/colormap.pcx")
        {
            commonPaletteEntry = entries[i];
            break;
        }
    }

    ReadPakEntryToBuffer(inputfile, commonPaletteEntry, pcx);
    GetPaletteFromPCX(pcx, pal);
}