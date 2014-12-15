#include <FileSystem.h>
#include <GDKError.h>
#include <FormatString.h>
#include "FileVolume.h"
#include "DirectoryFileVolume.h"

using namespace GDK;

static std::vector<std::shared_ptr<FileVolume>> g_volumes;

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

_Use_decl_annotations_
void GDK::FileSystem::MountVolume(const std::wstring &root, const std::wstring &logicalRoot, int32_t priority)
{
    // Add correct volume type
    if (StringEndsWith(root, L".zip"))
    {
        RAISE_EXCEPTION("ZIP files are not supported at this time.", 0);
    }
    else
    {
        g_volumes.push_back(std::shared_ptr<FileVolume>(new DirectoryFileVolume(root, logicalRoot, priority)));
    }

    // Sort content volumes by priority
    std::sort(std::begin(g_volumes), std::end(g_volumes), [&](std::shared_ptr<FileVolume> a, std::shared_ptr<FileVolume> b)
    {
        return a->GetPriority() > b->GetPriority();
    });
}

_Use_decl_annotations_
std::shared_ptr<std::istream> GDK::FileSystem::OpenStream(const std::wstring &filename)
{
    for (auto volume : g_volumes)
    {
        auto stream = volume->OpenFile(filename);
        if (stream != nullptr && stream->good())
        {
            return stream;
        }
    }

    // Throw exceptions if we fail to open a file
    std::wstring message = FormatString(L"Failed to open file {0}\n", filename.c_str());
    std::string messageAnsi(message.begin(), message.end());
    RAISE_EXCEPTION(messageAnsi.c_str(), -1);
}