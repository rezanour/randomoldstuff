#pragma once

#include "Platform.h"
#include "RuntimeObject.h"
#include "TextureContent.h"
#include "GeometryContent.h"
#include <istream>

namespace GDK
{
    class WorldContent : public RuntimeObject<WorldContent>
    {
    public:
        static std::shared_ptr<WorldContent> Create(_In_ std::istream &stream)
        {
            UNREFERENCED_PARAMETER(stream);
            return nullptr;
        }
    };

    namespace Content
    {
        // root - Path to ZIP file or directory for volume.
        // logicalRoot - The logical root of the volume. For example you could pass in "Media\Textures" and
        //               then this volume would only be used to load content that starts with "Media\Textures".
        // priority - A priority order for content to consider this volume. Higher priority volumes are checked first.
        void MountContentVolume(_In_ const std::wstring &root, _In_ const std::wstring &logicalRoot, _In_ int32_t priority);

        std::shared_ptr<TextureContent> LoadTextureContent(_In_ const std::wstring &name);
        std::shared_ptr<GeometryContent> LoadGeometryContent(_In_ const std::wstring &name);
        std::shared_ptr<WorldContent> LoadWorldContent(_In_ const std::wstring &name);
    }
}
