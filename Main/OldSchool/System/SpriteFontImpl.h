#pragma once

class SpriteFont : public BaseObject<SpriteFont>, public ISpriteFont
{
public:
    static std::shared_ptr<SpriteFont> Create(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_ uint32_t spriteFontDataSize, _In_ const void* spriteFontData);
    void Draw(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_z_ const wchar_t* text, _In_ int32_t x, _In_ int32_t y) override;
    uint32_t GetLineSpacing() override;
    uint32_t GetStringLength(_In_z_ const wchar_t* text) override;

    // Describes a single character glyph.
    struct Glyph
    {
        uint32_t Character;
        RECT Subrect;
        float XOffset;
        float YOffset;
        float XAdvance;
    };

private:
    SpriteFont(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, uint32_t spriteFontDataSize, const void* spriteFontData);

    Glyph const* FindGlyph(_In_ wchar_t character) const;
    void SetDefaultCharacter(_In_ wchar_t character);

    template<typename TAction>
    void ForEachGlyph(_In_z_ wchar_t const* text, TAction action);

private:
    std::shared_ptr<ITexture> _texture;
    std::vector<Glyph> glyphs;
    Glyph const* defaultGlyph;
    float lineSpacing;
};
