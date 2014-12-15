#pragma once

class Texture2D;
class Object;
class Camera;
class Scene;

enum class CombineType
{
    Add,
    Multiply
};

class Combine
{
public:
    Combine(ID3D11DeviceContext* context, const std::shared_ptr<Texture2D>& texture1, const std::shared_ptr<Texture2D>& texture2, const std::shared_ptr<Texture2D>& combine, CombineType type);

    void RenderFrame(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene);

private:
    CombineType _type;

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
        uint32_t Type;
        float Padding;
    };
    psPerFrameConstants _psPerFrameConstants;
    ComPtr<ID3D11Buffer> _psPerFrameConstantBuffer;

    std::shared_ptr<Texture2D> _texture1;
    std::shared_ptr<Texture2D> _texture2;
    std::shared_ptr<Texture2D> _combine;
};
