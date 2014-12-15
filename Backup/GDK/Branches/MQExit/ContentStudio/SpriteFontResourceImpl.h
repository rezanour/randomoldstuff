#pragma once

class SpriteFontResource : public ISpriteFontResource
{
public:
    SpriteFontResource();
    virtual ~SpriteFontResource();

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // ISpriteFontResource
    HRESULT Save(_In_ IStream* pStream);
    HRESULT Load(_In_ IStream* pStream);
    HRESULT GetTextureResource(_Deref_out_ ITextureResource** ppResource);

    HRESULT Initialize(LOGFONT* plogFont, bool antialias);

private:
    std::wstring AnsiToWide(const char* pstr);
    int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
    bool BitmapIsEmpty(Gdiplus::Bitmap* pbitmap, int x);
    HRESULT CropCharacter(Gdiplus::Bitmap* pbitmapIn, Gdiplus::Bitmap** ppbitmapOut);
    HRESULT RasterizeCharacter(char ch, bool antialias, Gdiplus::Bitmap** ppbitmap);

private:
    long _refcount;
    std::string _name;
    uint64 _id;

    std::wstring m_fontName;
    std::vector<RECT> m_glyphRects;
    std::vector<CHAR> m_glyphs;

    std::wstring m_spriteFontName;

    int m_minChar;
    int m_maxChar;
    Gdiplus::Bitmap*   m_pbitmap;
    Gdiplus::Font*     m_pfont;
    Gdiplus::Graphics* m_pgraphics;

    stde::com_ptr<ITextureResource> m_textureResource;
};

