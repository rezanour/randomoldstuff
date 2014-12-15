#pragma once

#include "GraphicsTypes.h"
#include "Texture.h"
#include "BaseShader.h"
#include <map>

namespace Lucid3D
{
    enum TextureUsage
    {
        Diffuse,
        NormalMap,
        SpecularMap,
        EnvironmentMap,
    };

    class Material;
    typedef stde::ref_counted_ptr<Material> MaterialPtr;

    class Material : stde::non_copyable, public GDK::RefCountedBase<GDK::IRefCounted>
    {
    public:
        static HRESULT Create(_In_ Renderer* pRenderer, _In_ IContentManagerPtr& spContentManager, _In_ uint64 contentId, _Out_ MaterialPtr& spMaterial);

        TexturePtr GetTexture(_In_ TextureUsage usage);
        ShaderPtr GetShader() const;

    private:
        Material(_In_ uint32 materialId, _In_ ShaderPtr& spShader);

        void SetTexture(_In_ TextureUsage usage, _In_ TexturePtr texture);

        typedef std::map<TextureUsage, TexturePtr> TextureTable;
        TextureTable _textures;

        uint32 _materialId;
        ShaderPtr _spShader;
        TexturePtr _nullTexture;
    };
}

