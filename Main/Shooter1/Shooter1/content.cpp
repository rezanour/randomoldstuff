#include "precomp.h"
#include "content.h"

template <typename ContentType>
static inline ContentType ContentLoadHelper(_In_z_ const char* path, _Inout_ std::map<std::string, ContentType>& cache, _In_ std::function<ContentType (const char*)> loader)
{
    // Each [path] indexer using char* will alloc a std::string.
    // Create 1 and use it in both places
    std::string thePath(path);

    auto it = cache.find(thePath);
    if (it == cache.end())
    {
        ContentType resource = loader(path);
        cache[thePath] = resource;
        return resource;
    }

    return it->second;
}

_Use_decl_annotations_
Texture Content::GetTexture(const char* path, bool supportsMips)
{
    return ContentLoadHelper<Texture>(path, _textureContent, [supportsMips] (const char* path) { return GetAssetLoader().LoadTextureFromFile(path, supportsMips); });
}

_Use_decl_annotations_
Geometry Content::GetGeometry(const char* path)
{
    return ContentLoadHelper<Geometry>(path, _geometryContent, [] (const char* path) { return GetAssetLoader().LoadGeometryFromFile(path); });
}

_Use_decl_annotations_
SpriteFont Content::GetSpriteFont(const char* path)
{
    return ContentLoadHelper<SpriteFont>(path, _spriteFontContent, [] (const char* path) { return GetAssetLoader().LoadSpriteFontFromFile(path); });
}

_Use_decl_annotations_
SingleSoundEffect Content::GetSoundEffect(const char* path)
{
    return ContentLoadHelper<SingleSoundEffect>(path, _soundEffectContent, [] (const char* path) { return GetAssetLoader().LoadSoundEffectFromFile(path); });
}
