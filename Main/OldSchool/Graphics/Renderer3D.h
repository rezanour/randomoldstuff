#pragma once

#include <GraphicsScene.h>

class GraphicsSystem;
class Texture;
class RenderPass;

class Renderer3D : public BaseObject<Renderer3D>
{
public:
    static std::shared_ptr<Renderer3D> Create(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context, _In_ const ComPtr<ID3D11RenderTargetView>& target, _In_ const std::shared_ptr<Texture>& depth);

    //
    // Internal
    //

    void DrawScene(_In_ const std::shared_ptr<IGraphicsScene>& scene, _In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection);

    // Commits output to the target
    void Render();

private:
    Renderer3D(_In_ const std::shared_ptr<GraphicsSystem>& graphics, _In_ const ComPtr<ID3D11DeviceContext>& context, _In_ const ComPtr<ID3D11RenderTargetView>& target, _In_ const std::shared_ptr<Texture>& depth);

private:
    std::weak_ptr<GraphicsSystem> _graphics;
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<ID3D11RenderTargetView> _target;

    // Rendering passes to produce a frame
    std::vector<std::shared_ptr<RenderPass>> _passes;

    // Textures
    std::shared_ptr<Texture> _depth;
    std::shared_ptr<Texture> _diffuse;
    std::shared_ptr<Texture> _normals;
    std::shared_ptr<Texture> _combined;
    std::shared_ptr<Texture> _combinedCopy; // For input into transparent phase
};
