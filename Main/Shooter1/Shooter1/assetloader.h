#pragma once

class Texture;
class SpriteFont;
class SingleSoundEffect;
class StaticLevelData;

struct TextureMetadata
{
    uint32_t    Width;
    uint32_t    Height;
    uint32_t    MipLevels;
    DXGI_FORMAT Format;
};

class AssetLoader : public TrackedObject<MemoryTag::AssetLoader>
{
public:
    void GetTextureMetadata(_In_z_ const char* filename, _Out_ TextureMetadata* metadata) const;
    void LoadMips(_In_z_ const char* source, uint32_t sourceStartMip, _In_ ID3D11Texture2D* dest, uint32_t destIndex, uint32_t destStartMip, uint32_t numMips) const;

    Texture LoadTextureFromFile(_In_z_ const char* filename, _In_ bool supportsMips) const;
    Texture LoadTextureFromMemory(_In_ uint32_t width, _In_ uint32_t height, _In_ uint32_t pitch, _In_ const uint8_t* data, _In_ DXGI_FORMAT format, _In_ uint8_t mipCount, _In_ bool supportsMips) const;

    void LoadMipsIntoLocation(_In_z_ const char* filename, uint32_t sourceStartingMip, uint32_t numMips, const ComPtr<ID3D11Texture2D>& dest, uint32_t destIndex, uint32_t destStartingMip) const;

    Geometry LoadGeometryFromFile(_In_z_ const char* filename) const;

    SpriteFont LoadSpriteFontFromFile(_In_z_ const char* filename) const;

    SingleSoundEffect LoadSoundEffectFromFile(_In_z_ const char* filename) const;

    StaticLevelData* LoadStaticLevel(_In_z_ const char* filename) const;

private:
    friend void AssetLoaderStartup();
    AssetLoader();
    AssetLoader(const AssetLoader&);

private:
};

void AssetLoaderStartup();
void AssetLoaderShutdown();

const AssetLoader& GetAssetLoader();
