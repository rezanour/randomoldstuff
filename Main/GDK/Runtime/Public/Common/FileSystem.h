#pragma once

#include "Platform.h"
#include <string>
#include <memory>

namespace GDK
{
    namespace FileSystem
    {
        // root - Path to ZIP file or directory for volume.
        // logicalRoot - The logical root of the volume. For example you could pass in "Media\Textures" and
        //               then this volume would only be used to load content that starts with "Media\Textures".
        // priority - A priority order for content to consider this volume. Higher priority volumes are checked first.
        void MountVolume(_In_ const std::wstring &root, _In_ const std::wstring &logicalRoot, _In_ int32_t priority);

        // Opens a stream to the given file in the volumes mounted
        std::shared_ptr<std::istream> OpenStream(_In_ const std::wstring &filename);
    }
}