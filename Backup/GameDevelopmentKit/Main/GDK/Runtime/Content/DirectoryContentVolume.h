#pragma once

#include "ContentVolume.h"
#include <unordered_set>

namespace GDK
{
    // Simple file system based volume
    class DirectoryContentVolume : public ContentVolume
    {
    public:
        DirectoryContentVolume(_In_ const std::wstring &root, _In_ const std::wstring &logicalRoot, _In_ int32_t priority);
        virtual std::shared_ptr<std::istream> OpenFile(_In_ const std::wstring &filename) override;
    };
}
