#include "Precomp.h"
#include "RuntimeTexture.h"

using namespace GDK;
using namespace GDK::Content;
using Microsoft::WRL::ComPtr;

namespace GDK {
namespace Graphics {

ComPtr<RuntimeTexture> GDKAPI RuntimeTexture::Create(_In_ DxGraphicsDevice* device, _In_ GDK::Content::ITextureResource* resource)
{
    CHECK_NOT_NULL(device, E_INVALIDARG);
    CHECK_NOT_NULL(resource, E_INVALIDARG);

    size_t width, height;
    CHECKHR(resource->GetSize(0, reinterpret_cast<uint32_t*>(&width), reinterpret_cast<uint32_t*>(&height)));

    const byte_t* pixels = nullptr;
    CHECKHR(resource->GetPixels(0, &pixels));

    return CreateFromData(device, width, height, pixels);
}

ComPtr<RuntimeTexture> GDKAPI RuntimeTexture::CreateFromData(_In_ DxGraphicsDevice* device, _In_ size_t width, _In_ size_t height, _In_ const byte_t* pixels)
{
    CHECK_NOT_NULL(device, E_INVALIDARG);
    CHECK_NOT_NULL(pixels, E_INVALIDARG);

    ComPtr<RuntimeTexture> texture = Make<RuntimeTexture>(L"");

    D3D11_TEXTURE2D_DESC desc = {0};
    desc.ArraySize = 1;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Width = static_cast<UINT>(width);
    desc.Height = static_cast<UINT>(height);
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA initData = {0};
    initData.pSysMem = pixels;
    initData.SysMemPitch = desc.Width * 4;
    initData.SysMemSlicePitch = 0;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Format = desc.Format;
    srvDesc.Texture2D.MipLevels = static_cast<UINT>(-1);
    srvDesc.Texture2D.MostDetailedMip = 0;

    CHECKHR(device->GetDevice()->CreateTexture2D(&desc, &initData, &texture->_texture));
    CHECKHR(device->GetDevice()->CreateShaderResourceView(texture->_texture.Get(), &srvDesc, &texture->_srv));

    return texture.Detach();
}

RuntimeTexture::RuntimeTexture(_In_ const std::wstring& name) :
    _name(name),
    _isBound(false)
{
}

void RuntimeTexture::Bind(_In_ DxGraphicsDevice* device)
{
    _isBound = true;
    device->GetContext()->PSSetShaderResources(0, 1, _srv.GetAddressOf());
}

void RuntimeTexture::Unbind(_In_ DxGraphicsDevice* device)
{
    _isBound = false;
    ID3D11ShaderResourceView* nulls[] = { nullptr, nullptr };
    device->GetContext()->PSSetShaderResources(0, 2, nulls);
}

} // Graphics
} // GDK
