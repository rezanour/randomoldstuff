#include <Precomp.h>
#include "GraphicsSystemImpl.h"
#include "GraphicsSceneImpl.h"
#include "TextureImpl.h"
#include "ZPrepass.h"
#include "GBufferPass.h"
#include "AmbientLightPass.h"
#include "DirectionalLightPass.h"
#include "PointLightPass.h"
#include "FullscreenPass.h"
#include "TransparentPass.h"
#include "DebugDrawPass.h"
#include "Renderer3D.h"

_Use_decl_annotations_
std::shared_ptr<Renderer3D> Renderer3D::Create(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context, const ComPtr<ID3D11RenderTargetView>& target, const std::shared_ptr<Texture>& depth)
{
    return std::shared_ptr<Renderer3D>(new Renderer3D(graphics, context, target, depth));
}

_Use_decl_annotations_
Renderer3D::Renderer3D(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context, const ComPtr<ID3D11RenderTargetView>& target, const std::shared_ptr<Texture>& depth) :
    _graphics(graphics), _context(context), _target(target), _depth(depth)
{
    ComPtr<ID3D11Device> device;
    _context->GetDevice(&device);

    auto& config = graphics->GetConfig();

    _diffuse = Texture::CreateTexture(device, config.Width, config.Height, DXGI_FORMAT_R8G8B8A8_UNORM, nullptr, 0, false, true);
    _normals = Texture::CreateTexture(device, config.Width, config.Height, DXGI_FORMAT_R8G8B8A8_UNORM, nullptr, 0, false, true);
    _combined = Texture::CreateTexture(device, config.Width, config.Height, DXGI_FORMAT_R8G8B8A8_UNORM, nullptr, 0, false, true);
    _combinedCopy = Texture::CreateTexture(device, config.Width, config.Height, DXGI_FORMAT_R8G8B8A8_UNORM, nullptr, 0, false, true);

#if defined(ENABLE_ZPREPASS)
    _passes.push_back(ZPrepass::Create(graphics, context, _depth));
#endif
    _passes.push_back(GBufferPass::Create(graphics, context, _diffuse, _normals, _depth));
    _passes.push_back(AmbientLightPass::Create(graphics, context, _diffuse, _combined));
    _passes.push_back(DirectionalLightPass::Create(graphics, context, _combined, _diffuse, _normals, _depth));
    _passes.push_back(PointLightPass::Create(graphics, context, _combined, _diffuse, _normals, _depth));

    // Transparency
    auto buildTransparency = BuildTransparentListPass::Create(graphics, context, _combinedCopy, _depth);
    _passes.push_back(buildTransparency);
    _passes.push_back(RenderTransparentPass::Create(graphics, context, _combined, _depth, _combinedCopy, buildTransparency));

    // Debug
    _passes.push_back(DebugDrawPass::Create(graphics, context, _combined, _depth));
}

void Renderer3D::DrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection)
{
    static const float diffuseClear[] = { 0, 0, 0, 0 };
    static const float normalClear[] = { 0, 0, 0, 0 };
    static const float lightClear[] = { 0, 0, 0, 0 };

    _context->ClearRenderTargetView(_diffuse->GetRenderTargetView().Get(), diffuseClear);
    _context->ClearRenderTargetView(_normals->GetRenderTargetView().Get(), normalClear);
    _context->ClearRenderTargetView(_combined->GetRenderTargetView().Get(), lightClear);
    _context->ClearDepthStencilView(_depth->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    for (auto& pass : _passes)
    {
        pass->DrawScene(scene, view, projection);
    }

#if 0
    // If there's any debug payload, render it
    if (static_cast<GraphicsScene*>(scene.get())->HasDebugContent())
    {
        static_cast<GraphicsScene*>(scene.get())->ClearDebugContent();
    }
#endif
}

void Renderer3D::Render()
{
    ComPtr<ID3D11Resource> resource;
    _target->GetResource(&resource);
    _context->CopyResource(resource.Get(), _combined->GetTexture().Get());
}
