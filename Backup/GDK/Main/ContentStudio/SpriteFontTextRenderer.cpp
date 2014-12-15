#include "stdafx.h"

CSpriteFontTextRenderer::CSpriteFontTextRenderer()
{

}

CSpriteFontTextRenderer::~CSpriteFontTextRenderer()
{

}

HRESULT CSpriteFontTextRenderer::Initialize(int pointSize, LPCWSTR faceName)
{
    HRESULT hr = S_OK;
    ID3D11Device* pDevice = DXUTGetD3D11Device();
    RECT r[255];
    WCHAR n[64];
    CHAR c[255];
    size_t cGlyphs = 0;
    stde::com_ptr<ISpriteFontResource> spResource;
    stde::com_ptr<IDirectXTextureResource> spTextureResource;
    hr = CreateSpriteFont(pointSize, faceName, &spResource);
    if (SUCCEEDED(hr))
    {
        hr = spResource->GetTextureResource((ITextureResource**)&spTextureResource);
    }

    if (SUCCEEDED(hr))
    {
        hr = spResource->GetNumGlyphs(&cGlyphs);
    }

    if (SUCCEEDED(hr))
    {
        hr = spResource->GetFontName(64, n);
    }

    if (SUCCEEDED(hr))
    {
        hr = spResource->GetGlyphRects(cGlyphs, r);
    }

    if (SUCCEEDED(hr))
    {
        hr = spResource->GetGlyphs(cGlyphs, c);
    }

    for(size_t i = 0; i < cGlyphs; i++)
    {
        _glyphtionary[c[i]] = r[i];
        if (c[i] == 'T')
        {
            _lineHeight = r[i].bottom;
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = spTextureResource->CreateShaderResourceView(pDevice, &_sprite);
    }

    return hr;
}

int CSpriteFontTextRenderer::LineHeight()
{
    return _lineHeight;
}

int CSpriteFontTextRenderer::MeasureText(LPCWSTR text)
{
    std::wstring ws = text;
    std::string s = WideToAnsi(ws);
    int width = 0;
    for (int i = 0; i < s.length(); i++)
    {
        CHAR c = s[i];
        RECT r = _glyphtionary[c];
        r.right = r.left + r.right;
        r.bottom = r.top + r.bottom;
        width += r.right - r.left;
    }

    return width;
}

void CSpriteFontTextRenderer::DrawText(DirectX::SpriteBatch& spriteBatch, int x, int y, LPCWSTR text, DirectX::XMVECTOR color)
{
    std::wstring ws = text;
    std::string s = WideToAnsi(ws);
    float fxPos = (float)x;
    for (int i = 0; i < s.length(); i++)
    {
        CHAR c = s[i];
        RECT r = _glyphtionary[c];
        r.right = r.left + r.right;
        r.bottom = r.top + r.bottom;
        spriteBatch.Draw(_sprite, DirectX::XMFLOAT2(fxPos, (float)y), &r, color, 0, DirectX::XMFLOAT2(0, 0), 1.0);
        fxPos += r.right - r.left;
    }
}

HRESULT CSpriteFontTextRenderer::CreateSpriteFont(int pointSize, LPCWSTR faceName, ISpriteFontResource** ppResource)
{
    HRESULT hr = S_OK;
    if (pointSize < 0 || !faceName || !ppResource)
    {
        return E_INVALIDARG;
    }

    HDC hDC = GetDC(::GetDesktopWindow());

    static LOGFONT lf   = {0};
    lf.lfHeight         = -MulDiv(pointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
    lf.lfWeight         = 400;
    lf.lfQuality        = 1;
    lf.lfPitchAndFamily = 34;
    lf.lfOutPrecision   = 3;
    lf.lfClipPrecision  = 2;
    StringCchCopyW( lf.lfFaceName, ARRAYSIZE(lf.lfFaceName), faceName );

    stde::com_ptr<ISpriteFontResource> spResource;
    hr = SpriteFontResourceServices::FromLOGFONT(&lf, true, &spResource);
    if (SUCCEEDED(hr))
    {
        *ppResource = spResource.detach();
    }

    if (hDC)
    {
        ReleaseDC(::GetDesktopWindow(), hDC);
    }

    return hr;
}

void CSpriteFontTextRenderer::Clear()
{
    _sprite = nullptr;
    _glyphtionary.clear();
}
