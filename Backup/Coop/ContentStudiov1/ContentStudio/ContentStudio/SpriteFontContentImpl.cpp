#include "stdafx.h"

#pragma warning(disable : 4482) // disables warning 'nonstandard extension used: enum...etc'

CSpriteFontContent::CSpriteFontContent() :
    m_minChar(0x20),
    m_maxChar(0x7F),
    m_pbitmap(nullptr),
    m_pfont(nullptr),
    m_pgraphics(nullptr)
{

}

CSpriteFontContent::~CSpriteFontContent()
{
    m_Textureblob.Release();
}

HRESULT CSpriteFontContent::GetTextureData(BYTE** ppData, DWORD* pcbSize)
{
    HRESULT hr = S_OK;

    // allocate memory large enough to fit texture data + magic number
    DWORD cbData = (DWORD)m_Textureblob.GetBufferSize();// + sizeof(DWORD); 
    BYTE* pData = new BYTE[cbData]; 

    *ppData = pData;
    *pcbSize = cbData;

    // write/copy magic number
    //DWORD* pMagicNumber = (DWORD*)pData;
    //*pMagicNumber = 0x20534444;

    // copy DDS data
    //pData += sizeof(DWORD);
    memcpy(pData, m_Textureblob.GetBufferPointer(), m_Textureblob.GetBufferSize());

    return hr;
}

ISpriteFontContentPtr CSpriteFontContent::FromFile(LPCWSTR filePath)
{
    HRESULT hr = S_OK;
    ISpriteFontContentPtr pSpriteFontContent;
    pSpriteFontContent = std::make_shared<CSpriteFontContent>();
    
    hr = pSpriteFontContent->LoadFromFile(filePath);
    if (FAILED(hr))
    {
        DEBUG_PRINT( "Failed to load spritefont content file '%ws', hr = 0x%lx ", filePath, hr);
        goto Exit;
    }

Exit:

    if FAILED(hr)
    {
        pSpriteFontContent.reset();
    }

    return pSpriteFontContent;
}

HRESULT CSpriteFontContent::SaveToFile(LPCWSTR filePath)
{
    HRESULT hr = S_OK;
    DWORD cbWritten = 0;
    IStream* pStream = NULL;
    SPRITEFONTCONTENTFILEHEADER header = {0};

    hr = FileStream::OpenFile(filePath, &pStream, true);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to create spritefont content file");

    header.dwSize           = sizeof(header);
    header.numGlyphs        = (DWORD)m_glyphs.size();
    header.offsetGlyphRects = header.dwSize;
    header.offsetTexture    = header.offsetGlyphRects + (sizeof(RECT) * header.numGlyphs);
    StringCchCopyW( header.FontName, ARRAYSIZE(header.FontName), m_fontName.c_str() );
    // write glyphtionary
    for (size_t i = 0; i < m_glyphs.size(); i++)
    {
        header.Glyphtionary[i] = m_glyphs[i];
    }

    // Write file header
    hr = pStream->Write(&header, sizeof(header), &cbWritten);

    // Write glyph rects
    for (size_t i = 0; i < m_glyphRects.size(); i++)
    {
        hr = pStream->Write(&m_glyphRects[i], sizeof(RECT), &cbWritten);
    }

    // Write texture
    hr = pStream->Write(m_Textureblob.GetBufferPointer(), (ULONG)m_Textureblob.GetBufferSize(), &cbWritten);

Exit:

    SAFE_RELEASE(pStream);

    return hr;
}

HRESULT CSpriteFontContent::LoadFromFile(LPCWSTR filePath)
{
    HRESULT hr = S_OK;
    DWORD cbRead = 0;
    IStream* pStream = NULL;
    STATSTG statstg = {0};
    SPRITEFONTCONTENTFILEHEADER header = {0};
    DWORD cbTextureData = 0;

    hr = FileStream::OpenFile(filePath, &pStream, false);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load spritefont content file");

    hr = pStream->Stat(&statstg, STGTY_STREAM);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to get stream size");

    hr = pStream->Read(&header, sizeof(header), &cbRead);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to read spritefont header");

    hr = FileStream::SeekStream(pStream, STREAM_SEEK_SET, header.offsetTexture);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to seek to spritefont texture data");

    cbTextureData = statstg.cbSize.LowPart - header.offsetTexture;

    m_Textureblob.Initialize(cbTextureData);

    hr = pStream->Read(m_Textureblob.GetBufferPointer(), (ULONG)m_Textureblob.GetBufferSize(), &cbRead);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to read texture data from spritefont texture data section into texture blob memory");

Exit:

    SAFE_RELEASE(pStream);

    return hr;
}

HRESULT CSpriteFontContent::LoadFromLOGFONT(LOGFONT* plogFont)
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
        if (SUCCEEDED(RasterizeCharacter(ch, &pbitmap)))
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

        //CLSID bmpEncoder;
        //GetEncoderClsid(L"image/bmp", &bmpEncoder);
        //pFinalBitmap->Save(L"tempSpriteFont.bmp", &bmpEncoder);

        CLSID pngEncoder;
        GetEncoderClsid(L"image/png", &pngEncoder);
        pFinalBitmap->Save(L"tempSpriteFont.png", &pngEncoder);

        DirectX::TexMetadata metadata;
        DirectX::ScratchImage scratchImage;
        hr = DirectX::LoadFromWICFile(L"tempSpriteFont.png", DirectX::WIC_FLAGS_FORCE_RGB, &metadata, scratchImage);
        hr = DirectX::SaveToDDSMemory(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::DDS_FLAGS_FORCE_DX10_EXT, m_Textureblob);

        scratchImage.Release();

        // FIX: Fix this cheesy temp file stuff...
        //DeleteFile(L"tempSpriteFont.bmp");

        delete pFinalBitmap;
    }

    //SAFE_DELETE(m_pfont);
    SAFE_DELETE(m_pbitmap);
    SAFE_DELETE(m_pgraphics);

    return hr;
}

std::wstring CSpriteFontContent::AnsiToWide(const char* pstr)
{
    std::string s = pstr;
    std::wstring w;

    std::locale loc("english");
    std::locale::global(loc);

    w.resize(s.size());
    transform(s.begin(),s.end(),w.begin(),btowc);

    return w;
}

int CSpriteFontContent::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
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

bool CSpriteFontContent::BitmapIsEmpty(Gdiplus::Bitmap* pbitmap, int x)
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

HRESULT CSpriteFontContent::CropCharacter(Gdiplus::Bitmap* pbitmapIn, Gdiplus::Bitmap** ppbitmapOut)
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

HRESULT CSpriteFontContent::RasterizeCharacter(char ch, Gdiplus::Bitmap** ppbitmap)
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

    bool antialias = false;

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

// Stolen from CTextureContent implementation until a proper refactor can take place..
HRESULT CSpriteFontContent::LoadTextureFromBuffer(size_t width, size_t height, DXGI_FORMAT format, size_t rowPitch, size_t slicePitch, BYTE* pixels)
{
    HRESULT hr = S_OK;
    DirectX::ScratchImage scratchImage; // texture in RAW form
    DirectX::Image image = {width, height, format, rowPitch, slicePitch, pixels};

    if (format == DXGI_FORMAT_R8G8B8A8_UNORM)
    {
        scratchImage.InitializeFromImage(image);
    }
    else
    {
        hr = DirectX::Convert(image, DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::TEX_FR_ROTATE0, 1.0f, scratchImage);
        GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to convert buffer to DXGI_FORMAT_R8G8B8A8_UNORM");
    }

    // Convert any DDS into a DX10 DDS
    hr = DirectX::SaveToDDSMemory(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::DDS_FLAGS_FORCE_DX10_EXT, m_Textureblob);
    GOTO_EXIT_IF_FAILED_MESSAGE(hr, "Failed to load dds file");

Exit:

    scratchImage.Release();

    return hr;
}

HRESULT CSpriteFontContent::GetSpriteFontName(std::wstring& name)
{
    name = m_spriteFontName;
    return S_OK;
}

HRESULT CSpriteFontContent::SetSpriteFontName(std::wstring& name)
{
    m_spriteFontName = name;
    return S_OK;
}