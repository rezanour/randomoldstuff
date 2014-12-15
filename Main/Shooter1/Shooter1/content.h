#pragma once

class Content
{
public:
    Texture GetTexture(_In_z_ const char* path, _In_ bool supportsMips);
    Geometry GetGeometry(_In_z_ const char* path);
    SpriteFont GetSpriteFont(_In_z_ const char* path);
    SingleSoundEffect GetSoundEffect(_In_z_ const char* path);

private:
    std::map<std::string, Texture> _textureContent;
    std::map<std::string, Geometry> _geometryContent;
    std::map<std::string, SpriteFont> _spriteFontContent;
    std::map<std::string, SingleSoundEffect> _soundEffectContent;
};
