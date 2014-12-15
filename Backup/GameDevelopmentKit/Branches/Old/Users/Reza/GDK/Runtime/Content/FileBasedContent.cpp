#include "FileBasedContent.h"
#include <TextureContent.h>

namespace GDK
{
        // TEMP
        class GeometryContent
        {
        };

    namespace ContentInternal
    {
        FileBasedContent::FileBasedContent(_In_ const std::wstring& rootFolder) :
            _rootFolder(rootFolder)
        {
            // ensure the root ends in a slash
            if (_rootFolder[_rootFolder.size() - 1] != L'\\')
            {
                _rootFolder.push_back(L'\\');
            }
        }

        std::shared_ptr<TextureContent> FileBasedContent::LoadTexture(_In_ const std::wstring& name)
        {
            std::wstring fullPath(_rootFolder);
            fullPath.append(
                (name[0] == L'\\') ? 
                    name.substr(1) :
                    name);

            std::ifstream input(fullPath.c_str(), std::ios_base::binary, _SH_DENYWR);
            return TextureContent::FromStream(input);
        }

        std::shared_ptr<GeometryContent> FileBasedContent::LoadGeometry(_In_ const std::wstring& name)
        {
            UNREFERENCED_PARAMETER(name);
            return nullptr;
        }
    }
}
