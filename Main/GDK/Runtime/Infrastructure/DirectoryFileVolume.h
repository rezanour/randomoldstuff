#pragma once

#include "FileVolume.h"

namespace GDK
{
    // Simple file system based volume
    class DirectoryFileVolume : public FileVolume
    {
    public:
        DirectoryFileVolume(_In_ const std::wstring &root, _In_ const std::wstring &logicalRoot, _In_ int32_t priority);
        virtual std::shared_ptr<std::istream> OpenFile(_In_ const std::wstring &filename) override;
    };
}
