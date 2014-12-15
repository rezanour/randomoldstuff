#pragma once

class Texture2D;
class Object;
class Camera;
class Scene;

class Luminance
{
public:
    Luminance(ID3D11DeviceContext* context, const std::shared_ptr<Texture2D>& source, const std::shared_ptr<Texture2D>& luminance);

    void RenderFrame(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene);

private:
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;
    ComPtr<ID3D11SamplerState> _linearSampler;

    // Full screen quad
    std::shared_ptr<Object> _quad;
    ComPtr<ID3D11InputLayout> _inputLayout;

    // constants
    struct psPerFrameConstants
    {
        XMFLOAT2 ScreenSize;
        XMFLOAT2 Padding;
    };
    psPerFrameConstants _psPerFrameConstants;
    ComPtr<ID3D11Buffer> _psPerFrameConstantBuffer;

    std::shared_ptr<Texture2D> _source;
    std::shared_ptr<Texture2D> _luminance;
};
