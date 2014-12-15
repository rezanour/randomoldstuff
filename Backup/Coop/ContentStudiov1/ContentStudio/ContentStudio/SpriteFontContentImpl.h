#pragma once

#pragma pack(push,1)
struct SPRITEFONTCONTENTFILEHEADER
{
    DWORD dwSize;
    WCHAR FontName[32];
    DWORD numGlyphs;
    CHAR  Glyphtionary[256];
    DWORD offsetGlyphRects;
    DWORD offsetTexture;
};
#pragma pack(pop)

class CSpriteFontContent : public ISpriteFontContent
{
public:
    CSpriteFontContent();
    virtual ~CSpriteFontContent();

    static ISpriteFontContentPtr FromFile(LPCWSTR filePath);

public:
    HRESULT SaveToFile(LPCWSTR filePath);
    HRESULT LoadFromFile(LPCWSTR filePath);
    HRESULT LoadFromLOGFONT(LOGFONT* plogFont);
    HRESULT GetSpriteFontName(std::wstring& name);
    HRESULT SetSpriteFontName(std::wstring& name);
    HRESULT GetTextureData(BYTE** ppData, DWORD* pcbSize);

private:
    std::wstring AnsiToWide(const char* pstr);
    int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
    bool BitmapIsEmpty(Gdiplus::Bitmap* pbitmap, int x);
    HRESULT CropCharacter(Gdiplus::Bitmap* pbitmapIn, Gdiplus::Bitmap** ppbitmapOut);
    HRESULT RasterizeCharacter(char ch, Gdiplus::Bitmap** ppbitmap);

    HRESULT LoadTextureFromBuffer(size_t width, size_t height, DXGI_FORMAT format, size_t rowPitch, size_t slicePitch, BYTE* pixels);

private:

    DirectX::Blob m_Textureblob;

    std::wstring m_fontName;
    std::vector<RECT> m_glyphRects;
    std::vector<CHAR> m_glyphs;

    std::wstring m_spriteFontName;

    int m_minChar;
    int m_maxChar;
    Gdiplus::Bitmap*   m_pbitmap;
    Gdiplus::Font*     m_pfont;
    Gdiplus::Graphics* m_pgraphics;

};