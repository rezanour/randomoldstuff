#include "stdafx.h"
using namespace Microsoft::WRL;

HRESULT Convert8bitBMPto32bit(std::vector<byte_t>& bmpData, std::vector<RGBQUAD>& bmpal, BITMAPINFOHEADER& bmih)
{
    std::vector<byte_t> bmpData32;
    for (size_t i = 0; i < bmpData.size();i++)
    {
        // Look for Quake II transparent color
        if (bmpal[bmpData[i]].rgbRed == 159 && bmpal[bmpData[i]].rgbGreen == 91 && bmpal[bmpData[i]].rgbBlue == 83)
        {
            bmpData32.push_back(0); // R
            bmpData32.push_back(0); // G
            bmpData32.push_back(0); // B
            bmpData32.push_back(0); // A
        }
        else
        {
            bmpData32.push_back(bmpal[bmpData[i]].rgbRed);   // R
            bmpData32.push_back(bmpal[bmpData[i]].rgbGreen); // G
            bmpData32.push_back(bmpal[bmpData[i]].rgbBlue);  // B
            bmpData32.push_back(255);                        // A
        }
    }

    bmih.biBitCount = 32;
    bmih.biSizeImage *= 4;

    bmpData.resize(bmpData32.size());
    memcpy(bmpData.data(), bmpData32.data(), bmpData32.size());

    return S_OK;
}

HRESULT SaveBMP(const wchar_t* path, std::vector<byte_t>& bmpData, std::vector<RGBQUAD>& bmpal, BITMAPINFOHEADER& bmih)
{
    HRESULT hr = S_OK;
    ComPtr<IStream> bmpFile;
    BITMAPFILEHEADER bmfh = {0};
    ULONG bytesWritten = 0;

    CHECKHR(SHCreateStreamOnFile(path, STGM_CREATE|STGM_READWRITE, &bmpFile), L"Error creating BMP file");

    bmfh.bfSize      = sizeof(BITMAPFILEHEADER);
    bmfh.bfType      = 0x4D42;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    if (bmih.biBitCount == 8)
    {
        bmfh.bfOffBits += (DWORD)bmpal.size();
    }

    CHECKHR(bmpFile->Write(&bmfh, sizeof(bmfh), &bytesWritten), L"Failed writing BMP file header");
    CHECKHR(bmpFile->Write(&bmih, sizeof(bmih), &bytesWritten), L"Failed writing BMP information header");
    if (bmih.biBitCount == 8)
    {
        CHECKHR(bmpFile->Write(bmpal.data(), (ULONG)bmpal.size() * sizeof(RGBQUAD), &bytesWritten), L"Failed writing BMP palette");
    }
    CHECKHR(bmpFile->Write(bmpData.data(), (ULONG)bmpData.size(), &bytesWritten), L"Failed writing BMP file data");

Exit:

    return hr;
}

