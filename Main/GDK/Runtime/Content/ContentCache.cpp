#include <ContentCache.h>
#include <GDKError.h>

namespace GDK
{
    std::shared_ptr<ContentCache> ContentCache::Create(_In_ const DeviceContext& deviceContext)
    {
        return std::shared_ptr<ContentCache>(GDKNEW ContentCache(deviceContext));
    }

    ContentCache::ContentCache(_In_ const DeviceContext& deviceContext) :
        _deviceContext(deviceContext)
    {
    }

    std::shared_ptr<Texture> ContentCache::GetTexture(_In_ const std::wstring& name)
    {
        if (!_textures.count(name))
        {
            _textures[name] = _deviceContext.graphicsDevice->CreateTexture(Content::LoadTextureContent(name));
        }

        return _textures[name];
    }

    std::shared_ptr<Geometry> ContentCache::GetGeometry(_In_ const std::wstring& name)
    {
        if (!_geometry.count(name))
        {
            _geometry[name] = _deviceContext.graphicsDevice->CreateGeometry(Content::LoadGeometryContent(name));
        }

        return _geometry[name];
    }

    std::shared_ptr<AudioClip> ContentCache::GetAudioClip(_In_ const std::wstring& name)
    {
        if (!_audio.count(name))
        {
            _audio[name] = _deviceContext.audioDevice->CreateAudioClip(Content::LoadAudioContent(name));
        }

        return _audio[name];
    }

    void ContentCache::Clear()
    {
        _textures.clear();
        _geometry.clear();
        _audio.clear();
    }
}
