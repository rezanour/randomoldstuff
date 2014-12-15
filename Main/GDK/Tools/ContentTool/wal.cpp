#include "wal.h"

void ConvertWALTo32bitBMP(_In_ std::vector<uint8_t>& wal, _In_ std::vector<bmp_paletteentry_t>& pal, _Inout_ std::vector<uint8_t>& bmp)
{
    bmp.clear();
    wal_fileheader_t* walHeader = (wal_fileheader_t*)wal.data();
    bmp_infoheader_t bmih;
    bmp_fileheader_t bmfh;
    uint8_t* walbits = wal.data() + walHeader->offset[0];

    // bitmap file header
    bmfh.size      = sizeof(bmp_fileheader_t);
    bmfh.type      = BitmapFileType;
    bmfh.reserved1 = 0;
    bmfh.reserved2 = 0;
    bmfh.offBits   = bmfh.size + sizeof(bmih);

    // bitmap info header
    bmih.size           = sizeof(bmp_infoheader_t);
    bmih.width          = walHeader->width;
    bmih.height         = walHeader->height;
    bmih.planes         = 1;
    bmih.bitCount       = 32;
    bmih.compression    = 0;
    bmih.sizeImage      = bmih.width * bmih.height * 4;
    bmih.xPelsPerMeter  = 0;
    bmih.yPelsPerMeter  = 0;
    bmih.clrUsed        = 0;
    bmih.clrImportant   = 0;

    // contruct a full bmp file in memory as the output
    bmp.resize(bmfh.offBits + bmih.sizeImage);

    uint8_t* copyData = bmp.data();
    memset(copyData, 0, bmp.size());
    memcpy(copyData, (void*)&bmfh, sizeof(bmfh)); copyData += sizeof(bmfh);
    memcpy(copyData, (void*)&bmih, sizeof(bmih)); copyData += sizeof(bmih);

    for (uint32_t i = 0; i < (uint32_t)(bmih.width * bmih.height);i++)
    {
        bmp_paletteentry_t palEntry;
        memset(&palEntry, 0, sizeof(palEntry));

        // Look for Quake II transparent color
        if (pal[walbits[i]].red == 159 && pal[walbits[i]].green == 91 && pal[walbits[i]].blue == 83)
        {
            // Transparent pixel detected
        }
        else
        {
            // Opaque pixel detected
            palEntry = pal[walbits[i]];
            palEntry.reserved = 255;
        }

        memcpy(copyData, &palEntry, sizeof(palEntry));
        copyData += sizeof(palEntry);
    }
}