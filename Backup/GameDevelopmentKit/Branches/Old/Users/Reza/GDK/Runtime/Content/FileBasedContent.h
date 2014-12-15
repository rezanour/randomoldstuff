#pragma once

#include <Content.h>

namespace GDK
{
    namespace ContentInternal
    {
        class FileBasedContent : public Content
        {
        public:
            FileBasedContent(_In_ const std::wstring& rootFolder);

            std::shared_ptr<TextureContent> LoadTexture(_In_ const std::wstring& name) override;
            std::shared_ptr<GeometryContent> LoadGeometry(_In_ const std::wstring& name) override;

        private:
            std::wstring _rootFolder;
        };
    }
}
