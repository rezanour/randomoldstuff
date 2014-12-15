#pragma once

#include "Platform.h"

namespace GDK
{
    class TextureContent;
    class GeometryContent;

    class Content : public RuntimeObject<Content>
    {
    public:
        static std::shared_ptr<Content> CreateFileBased(_In_ const std::wstring& rootDirectory);

        virtual std::shared_ptr<TextureContent> LoadTexture(_In_ const std::wstring& name) = 0;
        virtual std::shared_ptr<GeometryContent> LoadGeometry(_In_ const std::wstring& name) = 0;

    protected:
        Content();
    };
}
