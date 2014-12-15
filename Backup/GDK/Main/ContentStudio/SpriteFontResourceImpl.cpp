#include "stdafx.h"
#include <CoreServices\MemoryStream.h>

#pragma warning(disable : 4482) // disables warning 'nonstandard extension used: enum...etc'

SpriteFontResource::SpriteFontResource() :
    _refcount(1),
    m_minChar(0x20),
    m_maxChar(0x7F),
    m_pbitmap(nullptr),
    m_pfont(nullptr),
    m_pgraphics(nullptr)
{
    _id = CoreServices::CreateUniqueId();
}

SpriteFontResource::~SpriteFontResource()
{

}

HRESULT STDMETHODCALLTYPE SpriteFontResource::QueryInterface(_In_ REFIID iid, _Deref_out_ void** ppvObject)
{
    if (ppvObject == nullptr)
    {
        return E_INVALIDARG;
    }

    *ppvObject = nullptr;

    if (iid == __uuidof(IUnknown))
    {
        *ppvObject = static_cast<IUnknown*>(this);
        AddRef();
        return S_OK;
    }
    else if (iid == __uuidof(ISpriteFontResource))
    {
        *ppvObject = static_cast<ISpriteFontResource*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE SpriteFontResource::AddRef()
{
    return (ULONG)InterlockedIncrement(&_refcount);
}

ULONG STDMETHODCALLTYPE SpriteFontResource::Release()
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
    {
        delete this;
    }

    return res;
}

HRESULT SpriteFontResource::Save(_In_ IStream* pStream)
{
    if (!pStream)
    {
        return E_INVALIDARG;
    }

    if (!m_textureResource)
    {
        return E_UNEXPECTED;
    }

    HRESULT hr = S_OK;
    ULONG cbWritten = 0;

    SPRITEFONT_RESOURCE_FILEHEADER header = {0};
    header.Header.Version   = SPRITEFONT_RESOURCE_VERSION;
    header.Header.Id        = (__int64)_id;
    StringCchCopyA( header.Header.Name, ARRAYSIZE(header.Header.Name), _name.c_str() );
    header.numGlyphs        = (uint32_t)m_glyphs.size();
    header.offsetGlyphRects = sizeof(header);
    header.offsetTexture    = header.offsetGlyphRects + (sizeof(RECT) * header.numGlyphs);
    StringCchCopyW( header.fontName, ARRAYSIZE(header.fontName), m_fontName.c_str() );
    
    // write glyphtionary
    for (size_t i = 0; i < m_glyphs.size(); i++)
    {
        header.glyphtionary[i] = m_glyphs[i];
    }

    // Write file header
    hr = pStream->Write(&header, sizeof(header), &cbWritten);
    if (SUCCEEDED(hr))
    {
        // Write glyph rects
        for (size_t i = 0; i < m_glyphRects.size(); i++)
        {
            hr = pStream->Write(&m_glyphRects[i], sizeof(RECT), &cbWritten);
            if (FAILED(hr))
            {
                break;
            }
        }
    }
    
    if (SUCCEEDED(hr))
    {
        hr = m_textureResource->Save(pStream, TextureResourceSaveFormat::textureresource);
    }

    return hr;
}

HRESULT SpriteFontResource::Load(_In_ IStream* pStream)
{
    HRESULT hr = S_OK;
    ULONG cbRead = 0;

    SPRITEFONT_RESOURCE_FILEHEADER header = {0};
    if (pStream == nullptr)
    {
        return E_INVALIDARG;
    }
    
    if (SUCCEEDED(hr))
    {
        hr = pStream->Read(&header, sizeof(header), &cbRead);
    }

    // get the source font name used to generate the sprite font
    if (SUCCEEDED(hr))
    {
        m_spriteFontName = header.fontName;
    }

    // seek to glyph data
    if (SUCCEEDED(hr))
    {
        // Seek to the source rect information
        hr = CoreServices::FileStream::Seek(pStream, STREAM_SEEK_SET, header.offsetGlyphRects);
    }

    // read glyph information
    if (SUCCEEDED(hr))
    {
        // populate the glyph dictionary and source rects
        for (DWORD i = 0; i < header.numGlyphs; i++)
        {
            // read glyph source rect from stream and populate glyph map
            RECT glyphRect = {0};
            hr = pStream->Read(&glyphRect, sizeof(RECT), &cbRead);
            if (SUCCEEDED(hr))
            {
                m_glyphRects.push_back(glyphRect);
                m_glyphs.push_back(header.glyphtionary[i]);
            }
            else
            {
                break;
            }
        }
    }

    // Seek to the texture information
    if (SUCCEEDED(hr))
    {        
        hr = CoreServices::FileStream::Seek(pStream, STREAM_SEEK_SET, header.offsetTexture);
    }

    // Read texture resource
    if (SUCCEEDED(hr))
    {
        hr = TextureResourceServices::FromStream(pStream, &m_textureResource);
    }
    
    return hr;    
}

HRESULT SpriteFontResource::GetTextureResource(_Deref_out_ ITextureResource** ppResource)
{
    if (!m_textureResource)
    {
        return E_NOINTERFACE;
    }
    
    return m_textureResource->QueryInterface(IID_IUnknown, (void**)ppResource);
}

HRESULT SpriteFontResource::GetNumGlyphs(_Out_ size_t* pnumGlyphs)
{
    if (!pnumGlyphs)
    {
        return E_INVALIDARG;
    }

    *pnumGlyphs = m_glyphRects.size();

    return S_OK;
}

HRESULT SpriteFontResource::GetGlyphRects(_In_ size_t numGlyphRects, _In_ RECT* pGlyphRects)
{
    if (!pGlyphRects)
    {
        return E_INVALIDARG;
    }

    for (size_t i = 0; i < numGlyphRects; i++)
    {
        pGlyphRects[i] = m_glyphRects[i];
        if (i > m_glyphRects.size())
        {
            break;
        }
    }

    return S_OK;
}

HRESULT SpriteFontResource::GetGlyphs(_In_ size_t numGlyphs, _Out_ CHAR* pGlyphs)
{
    if (!pGlyphs)
    {
        return E_INVALIDARG;
    }

    for (size_t i = 0; i < numGlyphs; i++)
    {
        pGlyphs[i] = m_glyphs[i];
        if (i > m_glyphs.size())
        {
            break;
        }
    }

    return S_OK;
}

HRESULT SpriteFontResource::GetFontName(_In_ size_t cchName, _Out_ WCHAR* pName)
{
    if (!pName)
    {
        return E_INVALIDARG;
    }

    return StringCchCopyW( pName, cchName, m_spriteFontName.c_str() );
}

HRESULT SpriteFontResource::Initialize(LOGFONT* plogFont, bool antialias)
{
    HRESULT hr = S_OK;

    if (!plogFont)
    {
        return E_INVALIDARG;
    }

    if ((m_minChar >= m_maxChar) ||
    (m_minChar < 0) || (m_minChar > 0xFFFF) ||
    (m_maxChar < 0) || (m_maxChar > 0xFFFF))
    {
        return E_INVALIDARG;
    }

    // copy the source font's name
    m_fontName = plogFont->lfFaceName;
    m_spriteFontName = m_fontName;

    // create font, bitmap and graphics objects
    m_pfont = new Gdiplus::Font(GetDC(GetDesktopWindow()), plogFont);
    m_pbitmap = new Gdiplus::Bitmap(1, 1, PixelFormat32bppARGB);
    m_pgraphics = Gdiplus::Graphics::FromImage(m_pbitmap);

    std::vector<Gdiplus::Bitmap*> bitmaps;
    std::vector<int> xPositions;
    std::vector<int> yPositions;

    const int padding = 8;

    int width = padding;
    int height = padding;
    int lineWidth = padding;
    int lineHeight = padding;
    int count = 0;

    for (char ch = (char)m_minChar; ch < m_maxChar; ch++)
    {
        Gdiplus::Bitmap* pbitmap = NULL;
        if (SUCCEEDED(RasterizeCharacter(ch, antialias, &pbitmap)))
        {
            bitmaps.push_back(pbitmap);
            m_glyphs.push_back(ch);

            xPositions.push_back(lineWidth);
            yPositions.push_back(height);

            lineWidth += pbitmap->GetWidth() + padding;
            lineHeight = max(lineHeight, (int)pbitmap->GetHeight() + padding);

            // Output 16 glyphs per line, then wrap to the next line.
            if ((++count == 16) || (ch == m_maxChar - 1))
            {
                width = max(width, lineWidth);
                height += lineHeight;
                lineWidth = padding;
                lineHeight = padding;
                count = 0;
            }
        }
    }

    Gdiplus::Bitmap* pFinalBitmap = NULL;
    pFinalBitmap = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);

    if (pFinalBitmap)
    {
        Gdiplus::Graphics* pGraphics = Gdiplus::Graphics::FromImage(pFinalBitmap);
        if (pGraphics)
        {
            //pGraphics->Clear(Gdiplus::Color::Magenta);
            pGraphics->Clear(Gdiplus::Color::Transparent);
            pGraphics->SetCompositingMode(Gdiplus::CompositingMode::CompositingModeSourceCopy);

            for (size_t i = 0; i < bitmaps.size(); i++)
            {
                RECT glyphRect = { (LONG)xPositions[i], (LONG)yPositions[i], (LONG)bitmaps[i]->GetWidth(), (LONG)bitmaps[i]->GetHeight() };
                m_glyphRects.push_back(glyphRect);

                pGraphics->DrawImage(bitmaps[i], xPositions[i], yPositions[i]);
            }

            pGraphics->Flush();
            delete pGraphics;
        }

        IStream* pImageStream = nullptr;
        if (SUCCEEDED(CoreServices::MemoryStream::Create(0,&pImageStream)))
        {
            CLSID pngEncoder;
            GetEncoderClsid(L"image/png", &pngEncoder);
            pFinalBitmap->Save(pImageStream, &pngEncoder);
        
            hr = TextureResourceServices::FromStream(pImageStream, &m_textureResource);

            pImageStream->Release();
        }

        delete pFinalBitmap;
    }

    //SAFE_DELETE(m_pfont);
    SAFE_DELETE(m_pbitmap);
    SAFE_DELETE(m_pgraphics);

    return hr;
}

std::wstring SpriteFontResource::AnsiToWide(const char* pstr)
{
    std::string s = pstr;
    std::wstring w;

    std::locale loc("english");
    std::locale::global(loc);

    w.resize(s.size());
    transform(s.begin(),s.end(),w.begin(),btowc);

    return w;
}

int SpriteFontResource::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

   Gdiplus::GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

bool SpriteFontResource::BitmapIsEmpty(Gdiplus::Bitmap* pbitmap, int x)
{
    for (UINT y = 0; y < pbitmap->GetHeight(); y++)
    {
        Gdiplus::Color pixelColor;
        pbitmap->GetPixel(x, y, &pixelColor);

        if (pixelColor.GetA() != 0)
            return false;
    }

    return true;
}

HRESULT SpriteFontResource::CropCharacter(Gdiplus::Bitmap* pbitmapIn, Gdiplus::Bitmap** ppbitmapOut)
{
    HRESULT hr = S_OK;

    int cropLeft = 0;
    int cropRight = pbitmapIn->GetWidth() - 1;

    // Remove unused space from the left.
    while ((cropLeft < cropRight) && (BitmapIsEmpty(pbitmapIn, cropLeft)))
        cropLeft++;

    // Remove unused space from the right.
    while ((cropRight > cropLeft) && (BitmapIsEmpty(pbitmapIn, cropRight)))
        cropRight--;

    // Don't crop if that would reduce the glyph down to nothing at all!
    if (cropLeft == cropRight)
    {
        *ppbitmapOut = pbitmapIn;
        return S_OK;
    }

    // Add some padding back in.
    cropLeft = max(cropLeft - 1, 0);
    cropRight = min(cropRight + 1, (int)pbitmapIn->GetWidth() - 1);

    int width = cropRight - cropLeft + 1;

    // Crop the glyph.
    Gdiplus::Bitmap* pCroppedBitmap = new Gdiplus::Bitmap(width, pbitmapIn->GetHeight(), pbitmapIn->GetPixelFormat());
    if (pCroppedBitmap)
    {
        Gdiplus::Graphics* pGraphics = Gdiplus::Graphics::FromImage(pCroppedBitmap);
        if (pGraphics)
        {
            pGraphics->SetCompositingMode(Gdiplus::CompositingMode::CompositingModeSourceCopy);
            pGraphics->DrawImage(pbitmapIn, 0,0,cropLeft, 0, width, pbitmapIn->GetHeight(), Gdiplus::Unit::UnitPixel);
            pGraphics->Flush();

            delete pGraphics;
        }

        *ppbitmapOut = pCroppedBitmap;
    }

    return hr;
}

HRESULT SpriteFontResource::RasterizeCharacter(char ch, bool antialias, Gdiplus::Bitmap** ppbitmap)
{
    HRESULT hr = S_OK;

    if (!ppbitmap)
    {
        return E_INVALIDARG;
    }

    *ppbitmap = NULL;

    std::string s;
    s+= ch;
    std::wstring text = AnsiToWide(s.c_str());

    Gdiplus::SizeF size;
    Gdiplus::SizeF layoutsize;
    Gdiplus::PointF pt;
    Gdiplus::StringFormat stringFmt;
    stringFmt.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);

    layoutsize.Width = 100;
    layoutsize.Height = 100;

    m_pgraphics->MeasureString(text.c_str(), (INT)text.length(), m_pfont, layoutsize, &stringFmt, &size, (INT*)NULL, (INT*)NULL);

    int width = (int)ceil(size.Width);
    int height = (int)ceil(size.Height);

    Gdiplus::Bitmap* pbitmap = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
    if (pbitmap)
    {
        Gdiplus::Graphics* pGraphics = Gdiplus::Graphics::FromImage(pbitmap);
        if (pGraphics)
        {
            if (antialias)
            {
                pGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintAntiAliasGridFit);
            }
            else
            {
                pGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintSingleBitPerPixelGridFit);
            }

            pGraphics->Clear(Gdiplus::Color::Transparent);

            Gdiplus::Brush* pBrush = new Gdiplus::SolidBrush(Gdiplus::Color::White);
            if (pBrush)
            {
                Gdiplus::StringFormat* pFormat = new Gdiplus::StringFormat();
                if (pFormat)
                {
                    pFormat->SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
                    pFormat->SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentNear);

                    Gdiplus::PointF origin;
                    pGraphics->DrawString(text.c_str(), (int)text.length(), m_pfont, origin, pFormat, pBrush);

                    delete pFormat;
                }

                delete pBrush;
            }

            pGraphics->Flush();

            delete pGraphics;
        }

        CropCharacter(pbitmap, ppbitmap);
        // If the returned bitmap is different than than the
        // source bitmap, remember to delete it because a cropped
        // copy was returned.
        if (pbitmap != *ppbitmap)
        {
            delete pbitmap;
        }
    }

    return hr;
}
