#pragma once

struct IGraphicsSystem;

struct __declspec(novtable) ISpriteFont
{
    virtual void Draw(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_z_ const wchar_t* text, _In_ int32_t x, _In_ int32_t y) = 0;
    virtual uint32_t GetLineSpacing() = 0;
    virtual uint32_t GetStringLength(_In_z_ const wchar_t* text) = 0;
};

std::shared_ptr<ISpriteFont> CreateSpriteFont(_In_ const std::shared_ptr<IGraphicsSystem>& graphics, _In_ uint32_t spriteFontDataSize, _In_ const void* spriteFontData);