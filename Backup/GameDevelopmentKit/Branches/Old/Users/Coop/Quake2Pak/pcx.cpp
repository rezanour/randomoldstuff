#include "stdafx.h"

HRESULT ConvertToBMP(PCWSTR pcxPath, PCWSTR bmpPath)
{
    if (!pcxPath || !bmpPath)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    HANDLE pcxFile = INVALID_HANDLE_VALUE;
    HANDLE bmpFile = INVALID_HANDLE_VALUE;
    DWORD bytesRead = 0;
    DWORD bytesWritten = 0;
    BITMAPFILEHEADER bmfh = {0};
    BITMAPINFOHEADER bmih = {0};
    RGBQUAD bmpal[256] = {0};
    LARGE_INTEGER liFileSize = {0};
    BYTE* pcxFileData = NULL;
    BYTE* bmpData = NULL;
    PCXFILEHEADER pcxfh = {0};
    int scanlineSize = 0;
    BYTE* scanLineTemp = NULL;

    if (SUCCEEDED(hr))
    {
        pcxFile = CreateFile(pcxPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (pcxFile == INVALID_HANDLE_VALUE)
        {
            hr = HRESULT_FROM_WIN32(::GetLastError());
        }
    }

    if (SUCCEEDED(hr))
    {
        bmpFile = CreateFile(bmpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (bmpFile == INVALID_HANDLE_VALUE)
        {
            hr = HRESULT_FROM_WIN32(::GetLastError());
        }
    }

    if (SUCCEEDED(hr))
    {
        liFileSize.LowPart = GetFileSize(pcxFile, (LPDWORD)&liFileSize.HighPart);
        if (liFileSize.LowPart == INVALID_FILE_SIZE)
        {
            hr = HRESULT_FROM_WIN32(::GetLastError());
        }
    }

    if (SUCCEEDED(hr))
    {
        pcxFileData = new BYTE[liFileSize.QuadPart];
        if (!pcxFileData)
        {
            hr = E_OUTOFMEMORY;
        }
    }
    
    if (SUCCEEDED(hr))
    {
        if (!ReadFile(pcxFile, pcxFileData, liFileSize.LowPart, &bytesRead, NULL))
        {
            hr = HRESULT_FROM_WIN32(::GetLastError());
        }
    }

    if (SUCCEEDED(hr))
    {
        CopyMemory(&pcxfh, pcxFileData, sizeof(pcxfh));
    }

    if (SUCCEEDED(hr))
    {
        scanlineSize = pcxfh.planes * pcxfh.bpl;
        ldiv_t divResult = ldiv(scanlineSize, sizeof(long));
        if (divResult.rem > 0)
        {
            scanlineSize = (scanlineSize/sizeof(long) + 1) * sizeof(long);
        }
        
        bmih.biSize           = sizeof(BITMAPINFOHEADER);
        bmih.biWidth          = pcxfh.x2 - pcxfh.x1 + 1;
        bmih.biHeight         = pcxfh.y2 - pcxfh.y1 + 1;
        bmih.biPlanes         = pcxfh.planes;
        bmih.biBitCount       = pcxfh.bpp;
        bmih.biCompression    = BI_RGB;
        bmih.biSizeImage      = scanlineSize * bmih.biHeight;
        bmih.biXPelsPerMeter  = 0;
        bmih.biYPelsPerMeter  = 0;
        bmih.biClrUsed        = 0;
        bmih.biClrImportant   = 0;

        bmpData = new BYTE[bmih.biSizeImage];
        if (bmpData)
        {
            ZeroMemory(bmpData, bmih.biSizeImage);
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }
    }

    if (SUCCEEDED(hr))
    {
        long lDataPos = 0;
        long lPos = 128;
        int iX = 0;

        for ( int iY = 0; iY < bmih.biHeight; iY++ )
        {
            // Decompress the scan line
            for ( iX = 0; iX < pcxfh.bpl; )
            {
                UINT uiValue = pcxFileData[lPos++];
                if ( uiValue > 192 ) 
                {  
                    uiValue -= 192;  // Repeat count
                    BYTE pixelColor = pcxFileData[lPos++];

                    if ( iX <= bmih.biWidth )
                    {  						
                        for ( BYTE bRepeat = 0; bRepeat < uiValue; bRepeat++ )
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
                    if ( iX <= bmih.biWidth )
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

        // Flip bitmap
        scanLineTemp = new BYTE[bmih.biWidth];
        if (!scanLineTemp)
        {
            hr = E_OUTOFMEMORY;
        }

        if (SUCCEEDED(hr))
        {
            BYTE* pLine1 = bmpData;
            BYTE* pLine2 = bmpData + (bmih.biSizeImage - bmih.biWidth);
            while(pLine1 <= pLine2)
            {
                CopyMemory(scanLineTemp, pLine1, bmih.biWidth);
                memmove(pLine1, pLine2, bmih.biWidth);
                CopyMemory(pLine2, scanLineTemp, bmih.biWidth);

                pLine1 += bmih.biWidth;
                pLine2 -= bmih.biWidth;
            }
        }

        // Write the palette
        if ( pcxFileData[lPos++] == 12 )
        {
            for ( short Entry=0; Entry < ARRAYSIZE(bmpal); Entry++ )
            {
                bmpal[Entry].rgbRed       = pcxFileData[lPos++];
                bmpal[Entry].rgbGreen     = pcxFileData[lPos++];
                bmpal[Entry].rgbBlue      = pcxFileData[lPos++];
                bmpal[Entry].rgbReserved  = 0;
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        bmfh.bfSize      = sizeof(BITMAPFILEHEADER);
        bmfh.bfType      = 0x4D42;
        bmfh.bfReserved1 = 0;
        bmfh.bfReserved2 = 0;
        bmfh.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(bmpal);
    }

    if (SUCCEEDED(hr))
    {
        if (!WriteFile(bmpFile, &bmfh, sizeof(bmfh), &bytesWritten, NULL))
        {
            hr = HRESULT_FROM_WIN32(::GetLastError());
        }
    }

    if (SUCCEEDED(hr))
    {
        if (!WriteFile(bmpFile, &bmih, sizeof(bmih), &bytesWritten, NULL))
        {
            hr = HRESULT_FROM_WIN32(::GetLastError());
        }
    }

    if (SUCCEEDED(hr))
    {
        if (!WriteFile(bmpFile, bmpal, sizeof(bmpal), &bytesWritten, NULL))
        {
            hr = HRESULT_FROM_WIN32(::GetLastError());
        }
    }

    if (SUCCEEDED(hr))
    {
        if (!WriteFile(bmpFile, bmpData, bmih.biSizeImage, &bytesWritten, NULL))
        {
            hr = HRESULT_FROM_WIN32(::GetLastError());
        }
    }

    // always attempt to close any open files before exiting
    if (pcxFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(pcxFile);
    }

    if (bmpFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(bmpFile);
    }

    // free allocated memory
    if (pcxFileData)
    {
        delete [] pcxFileData;
    }

    if (bmpData)
    {
        delete [] bmpData;
    }
    
    return hr;
}

/*

void SaveBitmap(char* fileName, long width, long height, unsigned char* pData, long dataSize, BITMAPFILEHEADER** ppbitmap)
{
    BITMAPFILEHEADER bitmapFileHeader = {0};
    BITMAPINFOHEADER bitmapInfoHeader = {0};
    int p = 0;
    FILE* bmp = NULL;
    long b = 0;
    BYTE* pTemp = NULL;
    BYTE* pBitmapFile = NULL;

    if (fileName)
    {
        bmp = fopen(fileName, "wb");
    }

    // define the bitmap file header 
    bitmapFileHeader.bfSize      = sizeof(BITMAPFILEHEADER);
    bitmapFileHeader.bfType      = 0x4D42;
    bitmapFileHeader.bfReserved1 = 0;
    bitmapFileHeader.bfReserved2 = 0;
    bitmapFileHeader.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (ARRAYSIZE(Wl6Pal) * sizeof(RGBQUAD));
 
    if (bmp)
    {
        fwrite(&bitmapFileHeader, sizeof(bitmapFileHeader), 1, bmp);
    }

    // define the bitmap information header 
    bitmapInfoHeader.biSize          = sizeof(BITMAPINFOHEADER);
    bitmapInfoHeader.biPlanes        = 1;
    bitmapInfoHeader.biBitCount      = 8;
    bitmapInfoHeader.biCompression   = BI_RGB;
    bitmapInfoHeader.biSizeImage     = width * height;
    bitmapInfoHeader.biXPelsPerMeter = 0; 
    bitmapInfoHeader.biYPelsPerMeter = 0;
    bitmapInfoHeader.biClrUsed       = 0;
    bitmapInfoHeader.biClrImportant  = 0;
    bitmapInfoHeader.biWidth         = width;
    bitmapInfoHeader.biHeight        = -height;

    if (bmp)
    {
        fwrite(&bitmapInfoHeader, sizeof(bitmapInfoHeader), 1, bmp);
    }

    if (ppbitmap)
    {
        pBitmapFile = (BYTE*)malloc(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (ARRAYSIZE(Wl6Pal) * sizeof(RGBQUAD)) + dataSize);
        pTemp = pBitmapFile;
        
        memcpy(pTemp, &bitmapFileHeader, sizeof(bitmapFileHeader));
        pTemp+= sizeof(bitmapFileHeader);

        memcpy(pTemp, &bitmapInfoHeader, sizeof(bitmapInfoHeader));
        pTemp+= sizeof(bitmapInfoHeader);
    }

    for(p = 0; p < ARRAYSIZE(Wl6Pal); p++)
    {
        if (bmp)
        {
            // Write RGBQUAD
            fwrite(&Wl6Pal[p].b, sizeof(Wl6Pal[p].b), 1, bmp);     // RGB
            fwrite(&Wl6Pal[p].g, sizeof(Wl6Pal[p].g), 1, bmp);     // RGB
            fwrite(&Wl6Pal[p].r, sizeof(Wl6Pal[p].r), 1, bmp);     // RGB
            fwrite(&Wl6Pal[0].r, sizeof(Wl6Pal[0].r), 1, bmp); // Reserved
        }

        if (pTemp)
        {
            *pTemp = Wl6Pal[p].b; pTemp++;
            *pTemp = Wl6Pal[p].g; pTemp++;
            *pTemp = Wl6Pal[p].r; pTemp++;
            *pTemp = Wl6Pal[p].r; pTemp++;
        }
    }

    if (bmp)
    {
        fwrite(pData, dataSize, 1, bmp);
        fclose(bmp);
    }

    if (pTemp)
    {
        memcpy(pTemp, pData, dataSize);
        *ppbitmap = (BITMAPFILEHEADER*)pBitmapFile;
    }
}

*/