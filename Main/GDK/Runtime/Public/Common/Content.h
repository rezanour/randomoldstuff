#pragma once

#include <Platform.h>
#include <RuntimeObject.h>
#include <TextureContent.h>
#include <GeometryContent.h>
#include <GameWorldContent.h>
#include <AudioContent.h>

namespace GDK
{
    namespace Content
    {
        std::shared_ptr<TextureContent> LoadTextureContent(_In_ const std::wstring &name);
        std::shared_ptr<GeometryContent> LoadGeometryContent(_In_ const std::wstring &name);
        std::shared_ptr<GameWorldContent> LoadGameWorldContent(_In_ const std::wstring &name);
        std::shared_ptr<AudioContent> LoadAudioContent(_In_ const std::wstring &name);
    }
}
