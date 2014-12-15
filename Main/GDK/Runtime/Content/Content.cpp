#include <Content.h>
#include <GDKError.h>
#include <algorithm>
#include <FileSystem.h>

using namespace GDK;

_Use_decl_annotations_
std::shared_ptr<TextureContent> GDK::Content::LoadTextureContent(const std::wstring &name)
{
    return TextureContent::Create(*GDK::FileSystem::OpenStream(name).get());
}

_Use_decl_annotations_
std::shared_ptr<GeometryContent> GDK::Content::LoadGeometryContent(const std::wstring &name)
{
    return GeometryContent::Create(*GDK::FileSystem::OpenStream(name).get());
}

_Use_decl_annotations_
std::shared_ptr<GameWorldContent> GDK::Content::LoadGameWorldContent(const std::wstring &name)
{
    return GameWorldContent::Create(*GDK::FileSystem::OpenStream(name).get());
}

_Use_decl_annotations_
std::shared_ptr<AudioContent> GDK::Content::LoadAudioContent(const std::wstring &name)
{
    return AudioContent::Create(*GDK::FileSystem::OpenStream(name).get());
}