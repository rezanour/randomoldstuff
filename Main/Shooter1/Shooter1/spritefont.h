#pragma once

#include "assetloader.h"

class SpriteFont
{
public:
    SpriteFont() :
        _defaultGlyph(nullptr)
    {}

    SpriteFont(_In_ SpriteFontFileData* data) :
        _defaultGlyph(nullptr)
    {
        Glyph* glyphData = data->Glyphs;
        for (uint32_t i = 0; i < data->GlyphCount; i++)
        {
            _glyphs.push_back(*glyphData);
            glyphData++;
        }

        _lineSpacing = data->LineSpacing;
        SetDefaultCharacter((wchar_t)data->DefaultCharacter);

        _texture = GetAssetLoader().LoadTextureFromMemory(data->ImageData.Width, data->ImageData.Height, data->ImageData.Pitch, data->ImageData.Data, data->ImageData.Format, 1, false);
    }

    static SpriteFont Load(_In_ SpriteFontFileData* data)
    {
        return SpriteFont(data);
    }

    // Allow copies
    SpriteFont(const SpriteFont& other)
    {
        _texture = other._texture;
        _glyphs = other._glyphs;
        _lineSpacing = other._lineSpacing;
    }

    void Draw(_In_ const POINT& dest, _In_ const XMFLOAT4& color, _In_ const char* format, ...) const
    {
        char text[1024];

        va_list args;
        va_start(args, format);
        vsprintf_s(text, format, args);
        va_end(args);

        // Nasty conversion happening here.  Need to normalize string to be
        // ansi or wide, but not take a conversion hit on every frame.
        std::string t = text;
        std::wstring wtext(t.size(), L' ');
        std::copy(std::begin(t), std::end(t), std::begin(wtext));

        ForEachGlyph(wtext.c_str(), [&](Glyph const* glyph, float x1, float y1)
        {
            RECT destRect;
            destRect.left = dest.x + (int32_t)x1 + (int32_t)glyph->XOffset;
            destRect.top = dest.y + (int32_t)y1 + (int32_t)glyph->YOffset;
            destRect.right = destRect.left + (glyph->Subrect.right - glyph->Subrect.left);
            destRect.bottom = destRect.top + (glyph->Subrect.bottom - glyph->Subrect.top);
            GetGraphics().Draw2D(_texture, glyph->Subrect, destRect, color);
        });
    }

    //
    // Fullscreen draw allows rendering in full backbuffer coordinate space, instead of virtual canvas
    //
    void DrawFullScreen(_In_ const POINT& dest, _In_ const XMFLOAT4& color, _In_ const char* format, ...) const
    {
        char text[1024];

        va_list args;
        va_start(args, format);
        vsprintf_s(text, format, args);
        va_end(args);

        // Nasty conversion happening here.  Need to normalize string to be
        // ansi or wide, but not take a conversion hit on every frame.
        std::string t = text;
        std::wstring wtext(t.size(), L' ');
        std::copy(std::begin(t), std::end(t), std::begin(wtext));

        ForEachGlyph(wtext.c_str(), [&](Glyph const* glyph, float x1, float y1)
        {
            RECT destRect;
            destRect.left = dest.x + (int32_t)x1 + (int32_t)glyph->XOffset;
            destRect.top = dest.y + (int32_t)y1 + (int32_t)glyph->YOffset;
            destRect.right = destRect.left + (glyph->Subrect.right - glyph->Subrect.left);
            destRect.bottom = destRect.top + (glyph->Subrect.bottom - glyph->Subrect.top);
            GetGraphics().Draw2DFullScreen(_texture, glyph->Subrect, destRect, color);
        });
    }

    void DrawUsingEmbeddedMarkup(_In_ const POINT& dest, _In_ const XMFLOAT4* colors, _In_ const size_t numColors, _In_ const char* format, ...)
    {
        char text[1024];

        va_list args;
        va_start(args, format);
        vsprintf_s(text, format, args);
        va_end(args);

        // Nasty conversion happening here.  Need to normalize string to be
        // ansi or wide, but not take a conversion hit on every frame.
        std::string t = text;
        std::wstring wtext(t.size(), L' ');
        std::copy(std::begin(t), std::end(t), std::begin(wtext));

        // Split the string on TAB characters.  A TAB separates the next color change for embedded text rendering
        std::vector<std::wstring> splitStrings;
        std::vector<size_t> colorLookup;

        split(wtext, L'\t', splitStrings, colorLookup);
        POINT destCursor = dest;
        POINT currentDest = destCursor;

        if (splitStrings.size() == 0)
        {
            splitStrings.push_back(wtext);
            colorLookup.push_back(0);
        }

        for (size_t i = 0; i < splitStrings.size(); i++)
        {
            currentDest = destCursor;

            ForEachGlyph(splitStrings[i].c_str(), [&](Glyph const* glyph, float x1, float y1)
            {
                size_t colorIndex = colorLookup[i];

                if (colorIndex >= numColors)
                {
                    colorIndex = 0; 
                }

                RECT destRect;
                destRect.left = currentDest.x + (int32_t)x1 + (int32_t)glyph->XOffset;
                destRect.top = currentDest.y + (int32_t)y1 + (int32_t)glyph->YOffset;
                destRect.right = destRect.left + (glyph->Subrect.right - glyph->Subrect.left);
                destRect.bottom = destRect.top + (glyph->Subrect.bottom - glyph->Subrect.top);

                GetGraphics().Draw2D(_texture, glyph->Subrect, destRect, colors[colorIndex]);

                destCursor.x = destRect.right;
                destCursor.y = dest.y;
            });
        }

    }

    void split(_In_ const std::wstring& s, _In_ wchar_t c, _In_ std::vector<std::wstring>& v, _In_ std::vector<size_t>& colorLookup)
    {
        size_t colorIndex = 0;
        colorLookup.push_back(colorIndex);

        std::wstring::size_type i = 0;
        std::wstring::size_type j = s.find(c);
        wchar_t strIndex[2] = { 0, 0 };

        while (j != std::wstring::npos) 
        {
            v.push_back(s.substr(i, j - i));
            i = ++j;
            strIndex[0] = s.data()[i];
            colorLookup.push_back((size_t)_wtoi(strIndex));

            i = ++j;
            j = s.find(c, j);
            if (j == std::wstring::npos)
            {
                v.push_back(s.substr(i, s.length()));
            }
        }
    }

    inline uint32_t GetLineSpacing()
    {
        return (uint32_t)_lineSpacing;
    }

    inline uint32_t GetStringLength(_In_z_ const wchar_t* text)
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

private:

    Glyph const* FindGlyph(_In_ wchar_t character) const
    {
        auto glyph = std::lower_bound(_glyphs.begin(), _glyphs.end(), character);

        if (glyph != _glyphs.end() && glyph->Character == character)
        {
            return &*glyph;
        }

        if (_defaultGlyph)
        {
            return _defaultGlyph;
        }

        throw std::exception("Character not in font");
    }

    void SetDefaultCharacter(_In_ wchar_t character)
    {
        _defaultGlyph = nullptr;

        if (character)
        {
            _defaultGlyph = FindGlyph(character);
        }
    }

    template<typename TAction>
    void ForEachGlyph(_In_z_ wchar_t const* text, TAction action) const
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
                y += _lineSpacing;
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

private:
    Texture _texture;
    std::vector<Glyph> _glyphs;
    Glyph const* _defaultGlyph;
    float _lineSpacing;
};

static inline bool operator< (Glyph const& left, Glyph const& right)
{
    return left.Character < right.Character;
}

static inline bool operator< (wchar_t left, Glyph const& right)
{
    return left < right.Character;
}

static inline bool operator< (Glyph const& left, wchar_t right)
{
    return left.Character < right;
}