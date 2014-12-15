#pragma once

#include <Windows.h>
#include <GDK\Tools\commonresource.h>

#pragma warning(push)
#pragma warning(disable : 4005)
#include <stdint.h>
#pragma warning(pop)

#pragma pack(push,1)

const uint32_t SPRITEFONT_RESOURCE_VERSION = 0x20314653; // "SF1 "

struct SPRITEFONT_RESOURCE_FILEHEADER
{
    COMMON_RESOURCE_FILEHEADER Header;
    WCHAR    fontName[32];      // name of the font used to create this spritefont
    uint32_t numGlyphs;         // number of glyphs defined in spritefont
    CHAR     glyphtionary[256]; // dictionary of glyphs used in spritefont (numGlyphs specifies the total valid entries in this array)
    uint32_t offsetGlyphRects;  // array of RECT structures for each glyph location on the texture resource (numGlyphs specifies the total valid entries in this data block)
    uint32_t offsetTexture;     // offset to embedded texture resource data
};

// The SpriteFont Resource file is constructed as follows:

// [SPRITEFONT_RESOURCE_FILEHEADER ]  contains version, glyph layout information
// [TEXTURE_RESOURCE_FILEHEADER    ]  contains individual texture resource

#pragma pack(pop)

[uuid("0ae4e217-aa68-43e5-ba83-52d30922eb12")]
struct ISpriteFontResource : public IUnknown
{
    virtual HRESULT Save(_In_ IStream* pStream) = 0;
    virtual HRESULT Load(_In_ IStream* pStream) = 0;
    virtual HRESULT GetTextureResource(_Deref_out_ ITextureResource** ppResource) = 0;
    virtual HRESULT GetNumGlyphs(_Out_ size_t* pnumGlyphs) = 0;
    virtual HRESULT GetGlyphRects(_In_ size_t numGlyphRects, _In_ RECT* pGlyphRects) = 0;
    virtual HRESULT GetGlyphs(_In_ size_t numGlyphs, _Out_ CHAR* pGlyphs) = 0;
    virtual HRESULT GetFontName(_In_ size_t cchName, _Out_ WCHAR* pName) = 0;
};
