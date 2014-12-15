#pragma once

#include <Platform.h>
#include <RuntimeObject.h>
#include <Content.h>
#include <GraphicsDevice.h>
#include <AudioDevice.h>
#include <DeviceContext.h>
#include <map>

namespace GDK
{
    class Texture;
    class Geometry;
    class AudioClip;

    class ContentCache : public RuntimeObject<ContentCache>
    {
    public:
        static std::shared_ptr<ContentCache> Create(_In_ const DeviceContext& deviceContext);

        std::shared_ptr<Texture> GetTexture(_In_ const std::wstring& name);
        std::shared_ptr<Geometry> GetGeometry(_In_ const std::wstring& name);
        std::shared_ptr<AudioClip> GetAudioClip(_In_ const std::wstring& name);

        void Clear();

    private:
        ContentCache(_In_ const DeviceContext& deviceContext);

        DeviceContext _deviceContext;

        std::map<std::wstring, std::shared_ptr<Texture>> _textures;
        std::map<std::wstring, std::shared_ptr<Geometry>> _geometry;
        std::map<std::wstring, std::shared_ptr<AudioClip>> _audio;
    };
}
