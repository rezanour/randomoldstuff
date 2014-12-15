#pragma once

HRESULT Convert8bitBMPto32bit(std::vector<byte_t>& bmpData, std::vector<RGBQUAD>& bmpal, BITMAPINFOHEADER& bmih);
HRESULT SaveBMP(const wchar_t* path, std::vector<byte_t>& bmpData, std::vector<RGBQUAD>& bmpal, BITMAPINFOHEADER& bmih);
