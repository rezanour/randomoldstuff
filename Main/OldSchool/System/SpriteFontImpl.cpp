#include "Precomp.h"
#include "Texture.h"
#include "GraphicsSystem.h"
#include "SpriteFont.h"
#include "SpriteFontImpl.h"

static inline bool operator< (SpriteFont::Glyph const& left, SpriteFont::Glyph const& right)
{
    return left.Character < right.Character;
}

static inline bool operator< (wchar_t left, SpriteFont::Glyph const& right)
{
    return left < right.Character;
}

static inline bool operator< (SpriteFont::Glyph const& left, wchar_t right)
{
    return left.Character < right;
}

_Use_decl_annotations_
std::shared_ptr<SpriteFont> SpriteFont::Create(const std::shared_ptr<IGraphicsSystem>& graphics, uint32_t spriteFontDataSize, const void* spriteFontData)
{
    return std::shared_ptr<SpriteFont>(new SpriteFont(graphics, spriteFontDataSize, spriteFontData));
}

_Use_decl_annotations_
SpriteFont::SpriteFont(const std::shared_ptr<IGraphicsSystem>& graphics, uint32_t spriteFontDataSize, const void* spriteFontData)
{
    uint8_t* data = (uint8_t*)spriteFontData;
    
    // skip magic number
    data += (sizeof(uint8_t) * 8);
    
    // read number of glyphs
    uint32_t glyphCount = *((uint32_t*)data);
    data += sizeof(uint32_t);

    // read glyphs
    for (uint32_t i = 0; i < glyphCount; i++)
    {
        glyphs.push_back(*(Glyph*)data);
        data += sizeof(Glyph);
    }

    // Read font properties.
    lineSpacing = *((float*)data);
    data += sizeof(float);

    SetDefaultCharacter((wchar_t)*(uint32_t*)(data));
    data += sizeof(uint32_t);

    // Read the texture data.
    uint32_t textureWidth = *((uint32_t*)data); data += sizeof(uint32_t);
    uint32_t textureHeight = *((uint32_t*)data); data += sizeof(uint32_t);
    DXGI_FORMAT textureFormat = *((DXGI_FORMAT*)data); data += sizeof(DXGI_FORMAT);
    uint32_t textureStride = *((uint32_t*)data); data += sizeof(uint32_t);
    /* uint32_t textureRows = *((uint32_t*)data);*/ data += sizeof(uint32_t);

    uint8_t* textureData = (uint8_t*)data; // textureStride * textureRows;

    _texture = graphics->CreateTexture(textureWidth, textureHeight, textureFormat, textureData, textureStride, false);

    UNREFERENCED_PARAMETER(spriteFontDataSize);
}

_Use_decl_annotations_
void SpriteFont::Draw(const std::shared_ptr<IGraphicsSystem>& graphics, const wchar_t* text, int32_t x, int32_t y)
{
    ForEachGlyph(text, [&](Glyph const* glyph, float x1, float y1)
    {
        RECT destRect;
        destRect.left = x + (int32_t)x1 + (int32_t)glyph->XOffset;
        destRect.top = y + (int32_t)y1 + (int32_t)glyph->YOffset;
        destRect.right = destRect.left + (glyph->Subrect.right - glyph->Subrect.left);
        destRect.bottom = destRect.top + (glyph->Subrect.bottom - glyph->Subrect.top);
        graphics->DrawImage(_texture, glyph->Subrect, destRect);
    });
}

uint32_t SpriteFont::GetLineSpacing()
{
    return (uint32_t)lineSpacing;
}

_Use_decl_annotations_
uint32_t SpriteFont::GetStringLength(const wchar_t* text)
{
    uint32_t result = 0;
    ForEachGlyph(text, [&](Glyph const* glyph, float x1, float y1)
    {
        UNREFERENCED_PARAMETER(x1);
        UNREFERENCED_PARAMETER(y1);
        result += ((glyph->Subrect.right - glyph->Subrect.left) + (int32_t)glyph->XOffset);
    });

    return result;
}

// Looks up the requested glyph, falling back to the default character if it is not in the font.
_Use_decl_annotations_
SpriteFont::Glyph const* SpriteFont::FindGlyph(wchar_t character) const
{
    auto glyph = std::lower_bound(glyphs.begin(), glyphs.end(), character);

    if (glyph != glyphs.end() && glyph->Character == character)
    {
        return &*glyph;
    }

    if (defaultGlyph)
    {
        return defaultGlyph;
    }

    throw std::exception("Character not in font");
}

// Sets the missing-character fallback glyph.
_Use_decl_annotations_
void SpriteFont::SetDefaultCharacter(wchar_t character)
{
    defaultGlyph = nullptr;

    if (character)
    {
        defaultGlyph = FindGlyph(character);
    }
}

// The core glyph layout algorithm, shared between DrawString and MeasureString.
template<typename TAction>
_Use_decl_annotations_
void SpriteFont::ForEachGlyph(_In_z_ wchar_t const* text, TAction action)
{
    float x = 0;
    float y = 0;

    for (; *text; text++)
    {
        wchar_t character = *text;

        switch (character)
        {
        case '\r':
            // Skip carriage returns.
            continue;

        case '\n':
            // New line.
            x = 0;
            y += lineSpacing;
            break;

        default:
            // Output this character.
            auto glyph = FindGlyph(character);

            x += glyph->XOffset;

            if (x < 0)
                x = 0;

            if (!iswspace(character))
            {
                action(glyph, x, y);
            }

            x += glyph->Subrect.right - glyph->Subrect.left + glyph->XAdvance;
            break;
        }
    }
}

_Use_decl_annotations_
std::shared_ptr<ISpriteFont> CreateSpriteFont(const std::shared_ptr<IGraphicsSystem>& graphics, uint32_t spriteFontDataSize, const void* spriteFontData)
{
    return SpriteFont::Create(graphics, spriteFontDataSize, spriteFontData);
}