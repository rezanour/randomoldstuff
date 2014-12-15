#pragma once

class Texture2D;
class Object;
class Camera;
class Scene;
class Light;

class Blur
{
public:
    Blur(ID3D11DeviceContext* context);

    void BlurSurface(const std::shared_ptr<Texture2D>& source, const std::shared_ptr<Texture2D>& target);

private:
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;
    ComPtr<ID3D11InputLayout> _inputLayout;
    ComPtr<ID3D11SamplerState> _linearSampler;

    std::shared_ptr<Object> _quad;

    struct psConstants
    {
        XMFLOAT2 ScreenSize;
        float FlipXY;
        float Padding;
    };
    psConstants _psConstants;
    ComPtr<ID3D11Buffer> _psConstantBuffer;
};
