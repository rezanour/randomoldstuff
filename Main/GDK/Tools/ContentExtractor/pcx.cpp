#include "stdafx.h"
using namespace Microsoft::WRL;

HRESULT SavePCXFileAsTextureContent(const wchar_t* path, const wchar_t* filePath)
{
    if (!path || !filePath)
    {
        return E_INVALIDARG;
    }

    std::vector<byte_t> pcxData;
    HRESULT hr = S_OK;
    ComPtr<IStream> pcxFile;
    ULONG bytesRead = 0;

    CHECKHR(SHCreateStreamOnFile(filePath, STGM_READ, &pcxFile), L"Error opening PCX file");
    pcxData.resize(GetStreamSize(pcxFile.Get()));
    CHECKHR(pcxFile->Read(pcxData.data(), (ULONG)pcxData.size(), &bytesRead), L"Error reading entire PCX file");
    CHECKHR(SavePCXAsTextureContent(path, " ", pcxData.data(), (uint32_t)pcxData.size()), L"Error saving Texture content from PCX file");
Exit:

    return hr;
}

HRESULT ConvertPCXToBMP(PCXFILEHEADER* header, byte_t* data, uint32_t size, std::vector<byte_t>& bmpData, std::vector<RGBQUAD>& bmpal, BITMAPINFOHEADER& bmheader, bool flipImage)
{
    DWORD bytesRead = 0;
    DWORD bytesWritten = 0;
    BITMAPINFOHEADER bmih = {0};
    bmpal.resize(256);
    LARGE_INTEGER liFileSize = {0};
    int scanlineSize = 0;
    std::vector<byte_t> scanLineTemp;

    scanlineSize = header->planes * header->bpl;
    //ldiv_t divResult = ldiv(scanlineSize, sizeof(long));
    //if (divResult.rem > 0)
    //{
    //    scanlineSize = (scanlineSize/sizeof(long) + 1) * sizeof(long);
    //}
        
    bmih.biSize           = sizeof(BITMAPINFOHEADER);
    bmih.biWidth          = header->x2 - header->x1 + 1;
    bmih.biHeight         = header->y2 - header->y1 + 1;
    bmih.biPlanes         = header->planes;
    bmih.biBitCount       = header->bpp;
    bmih.biCompression    = BI_RGB;
    bmih.biSizeImage      = scanlineSize * bmih.biHeight;
    bmih.biXPelsPerMeter  = 0;
    bmih.biYPelsPerMeter  = 0;
    bmih.biClrUsed        = 0;
    bmih.biClrImportant   = 0;

    bmpData.resize(bmih.biSizeImage);

    long lDataPos = 0;
    long lPos = 128;
    int iX = 0;

    for ( int iY = 0; iY < bmih.biHeight; iY++ )
    {
        // Decompress the scan line
        for ( iX = 0; iX < header->bpl; )
        {
            UINT uiValue = data[lPos++];
            if ( uiValue > 192 ) 
            {
                uiValue -= 192;  // Repeat count
                BYTE pixelColor = data[lPos++];

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

    if (flipImage)
    {
        // Flip bitmap
        scanLineTemp.resize(bmih.biWidth);
        BYTE* pLine1 = bmpData.data();
        BYTE* pLine2 = bmpData.data() + (bmih.biSizeImage - bmih.biWidth);
        while(pLine1 <= pLine2)
        {
            CopyMemory(scanLineTemp.data(), pLine1, bmih.biWidth);
            memmove(pLine1, pLine2, bmih.biWidth);
            CopyMemory(pLine2, scanLineTemp.data(), bmih.biWidth);

            pLine1 += bmih.biWidth;
            pLine2 -= bmih.biWidth;
        }
    }

    // Write the palette
    if ( data[lPos++] == 12 )
    {
        for ( short Entry=0; Entry < (short)bmpal.size(); Entry++ )
        {
            bmpal[Entry].rgbRed       = data[lPos++];
            bmpal[Entry].rgbGreen     = data[lPos++];
            bmpal[Entry].rgbBlue      = data[lPos++];
            bmpal[Entry].rgbReserved  = 0;
        }
    }

    bmheader = bmih;

    return S_OK;
}

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

HRESULT SavePCXAsTextureContent(const wchar_t* path, const char* name, byte_t* data, uint32_t size)
{
    if (!path || !data || !size)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    
    std::shared_ptr<GDK::TextureContent> textureContent;
    size_t numBytes = 0;
    std::unique_ptr<byte_t[]> pixels;
    std::unique_ptr<GDK::RectangleF[]> frames(new GDK::RectangleF[1]);
    std::ofstream resourceFile(path,std::ios_base::binary);

    ComPtr<IStream> bmpFile;
    std::vector<byte_t> bmpData;
    std::vector<RGBQUAD> bmpal;
    BITMAPINFOHEADER bmih = {0};

    ULONG bytesWritten = 0;
    PCXFILEHEADER* header = (PCXFILEHEADER*)data;
    printf("PCX details\n"
            "  name:     %s\n"
            "  id:       %d\n"
            "  version:  %d\n"
            "  encoding: %d\n"
            "  bpp:      %d\n"
            "  x1:       %d\n"
            "  y1:       %d\n"
            "  x2:       %d\n"
            "  y2:       %d\n"
            "  hres:     %d\n"
            "  yres:     %d\n"
            "  planes:   %d\n"
            "  bpl:      %d\n"
            "  pal:      %d\n", name, header->id, header->version, header->encoding, header->bpp, 
                                header->x1, header->y1, header->x2, header->y2, header->hres,
                                header->yres, header->planes, header->bpl, header->pal);

    CHECKHR(ConvertPCXToBMP(header, data, size, bmpData, bmpal, bmih), L"Failed to conver PCX to BMP");
    CHECKHR(Convert8bitBMPto32bit(bmpData, bmpal, bmih), L"Failed to convert 8-bit data to 32-bit BMP");

    numBytes = bmih.biWidth * bmih.biHeight * GDK::TextureContent::BytesPerPixel(GDK::TextureFormat::R8G8B8A8);
    pixels.reset(new byte_t[numBytes]);
    memcpy_s(pixels.get(), numBytes, bmpData.data(), numBytes);
    frames.get()[0] = GDK::RectangleF(0.0f, 0.0f, 1.0f, 1.0f);

    textureContent = GDK::TextureContent::Create(bmih.biWidth, bmih.biHeight, GDK::TextureFormat::R8G8B8A8, pixels, 1, frames);
    textureContent->Save(resourceFile);

Exit:

    return hr;
}
