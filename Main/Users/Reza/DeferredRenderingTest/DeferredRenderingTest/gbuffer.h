#pragma once

class Texture2D;
class Object;
class Camera;
class Scene;

class GBuffer
{
public:
    GBuffer(ID3D11DeviceContext* context, const std::shared_ptr<Texture2D>& diffuse, const std::shared_ptr<Texture2D>& normals, const std::shared_ptr<Texture2D>& depth);

    void RenderFrame(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene);

private:
    void BeginFrame(const std::shared_ptr<Camera>& camera);
    void RenderObjects(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene);

    void Bind();
    void Clear();

private:
    ComPtr<ID3D11DeviceContext> _context;

    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;
    ComPtr<ID3D11InputLayout> _inputLayoutPositionNormal;

    ComPtr<ID3D11Buffer> _vsPerFrameConstantBuffer;
    ComPtr<ID3D11Buffer> _vsPerObjectConstantBuffer;

    std::shared_ptr<Texture2D> _diffuse;
    std::shared_ptr<Texture2D> _normals;
    std::shared_ptr<Texture2D> _depth;

    ComPtr<ID3D11ShaderResourceView> _normalMap;
    ComPtr<ID3D11SamplerState> _pointSampler;

    // constants
    struct vsPerFrameConstantData
    {
        XMFLOAT4X4 View;
        XMFLOAT4X4 Projection;
    };
    vsPerFrameConstantData _vsPerFrameConstants;

    struct vsPerObjectConstantData
    {
        XMFLOAT4X4 World;
    };
    vsPerObjectConstantData _vsPerObjectConstants;

    // temporary cache reused each frame to avoid allocations
    std::vector<std::shared_ptr<Object>> _payload;
};
