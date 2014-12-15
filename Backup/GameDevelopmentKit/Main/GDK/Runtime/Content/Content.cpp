#include <Content.h>
#include <GDKError.h>
#include "ContentVolume.h"
#include "DirectoryContentVolume.h"
#include <algorithm>

using namespace GDK;

static std::vector<std::shared_ptr<ContentVolume>> g_contentVolumes;

static bool StringEndsWith(_In_ std::wstring search, _In_ std::wstring ending)
{
    if (ending.size() > search.size())
    {
        return false;
    }

    // convert strings to lowercase
    std::transform(std::begin(search), std::end(search), std::begin(search), ::toupper);
    std::transform(std::begin(ending), std::end(ending), std::begin(ending), ::toupper);

    // check for the suffic
    return std::equal(ending.rbegin(), ending.rend(), search.rbegin());
}

static std::shared_ptr<std::istream> OpenStream(_In_ const std::wstring &filename)
{
    for (auto itr = std::begin(g_contentVolumes); itr < std::end(g_contentVolumes); itr++)
    {
        auto stream = (*itr)->OpenFile(filename);
        if (stream != nullptr)
        {
            return stream;
        }
    }

    return nullptr;
}

_Use_decl_annotations_
void GDK::Content::MountContentVolume(const std::wstring &root, const std::wstring &logicalRoot, int32_t priority)
{
    // Add correct volume type
    if (StringEndsWith(root, L".zip"))
    {
        RAISE_EXCEPTION("ZIP files are not supported at this time.", 0);
    }
    else
    {
        g_contentVolumes.push_back(std::shared_ptr<ContentVolume>(new DirectoryContentVolume(root, logicalRoot, priority)));
    }

    // Sort content volumes by priority
    std::sort(std::begin(g_contentVolumes), std::end(g_contentVolumes), [&](std::shared_ptr<ContentVolume> a, std::shared_ptr<ContentVolume> b)
    {
        return a->GetPriority() > b->GetPriority();
    });
}

_Use_decl_annotations_
std::shared_ptr<TextureContent> GDK::Content::LoadTextureContent(const std::wstring &name)
{
    auto stream = OpenStream(name);
    CHECK_NOT_NULL(stream);
    CHECK_TRUE(stream->good());
    return TextureContent::Create(*stream);
}

_Use_decl_annotations_
std::shared_ptr<GeometryContent> GDK::Content::LoadGeometryContent(const std::wstring &name)
{
    auto stream = OpenStream(name);
    CHECK_NOT_NULL(stream);
    CHECK_TRUE(stream->good());
    return GeometryContent::Create(*stream);
}

_Use_decl_annotations_
std::shared_ptr<WorldContent> GDK::Content::LoadWorldContent(const std::wstring &name)
{
    auto stream = OpenStream(name);
    CHECK_NOT_NULL(stream);
    CHECK_TRUE(stream->good());
    return WorldContent::Create(*stream);
}
