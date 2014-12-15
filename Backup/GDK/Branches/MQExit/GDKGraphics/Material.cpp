#include "StdAfx.h"
#include "Material.h"
#include <CoreServices\Configuration.h>
#include "Renderer.h"

using namespace Lucid3D;
using namespace GDK;
using namespace CoreServices;

Material::Material(_In_ uint32 materialId, _In_ ShaderPtr& spShader)
    : _materialId(materialId), _spShader(spShader)
{
}

HRESULT Material::Create(_In_ Renderer* pRenderer, _In_ IContentManagerPtr& spContentManager, _In_ uint64 contentId, _Out_ MaterialPtr& spMaterial)
{
    HRESULT hr = S_OK;

    stde::com_ptr<IStream> spStream;
    ConfigurationPtr spProperties;
    ShaderPtr spShader;

    CHECKHR(spContentManager->GetStream(contentId, &spStream));
    CHECKHR(Configuration::Load(spStream, &spProperties));

    uint32 materialId = spProperties->GetValue<uint32>("MaterialId", 0);
    spMaterial.attach(new Material(materialId, pRenderer->GetShaderForMaterial(materialId)));

    {
        uint64 diffuseId = spProperties->GetValue<uint64>("Diffuse", 0);
        if (diffuseId > 0)
        {
            TexturePtr spTexture;
            CHECKHR(Texture::Create(pRenderer, spContentManager, diffuseId, true, spTexture));
            spMaterial->SetTexture(TextureUsage::Diffuse, spTexture);
        }
    }

EXIT
    if (FAILED(hr))
    {
        spMaterial.reset();
    }
    return hr;
}

TexturePtr Material::GetTexture(_In_ TextureUsage usage)
{
    TextureTable::iterator it = _textures.find(usage);
    return (it != _textures.end()) ? it->second : _nullTexture;
}

ShaderPtr Material::GetShader() const
{
    return _spShader;
}

void Material::SetTexture(_In_ TextureUsage usage, _In_ TexturePtr texture)
{
    _textures[usage] = texture;
}

