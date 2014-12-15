#pragma once

class Texture2D;
class Object;
class Camera;
class Scene;
class Light;
class Blur;

class DirectionalLightShadows
{
public:
    DirectionalLightShadows(ID3D11DeviceContext* context);

    void BuildShadowmaps(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene);

private:
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;
    ComPtr<ID3D11InputLayout> _inputLayout;

    std::shared_ptr<Texture2D> _shadowDepth;
    std::shared_ptr<Texture2D> _blurScratch;
    std::shared_ptr<Blur> _blur;

    struct vsPerLightConstants
    {
        XMFLOAT4X4 View;
        XMFLOAT4X4 Projection;
    };
    vsPerLightConstants _vsPerLightConstants;
    ComPtr<ID3D11Buffer> _vsPerLightConstantBuffer;

    struct vsPerObjectConstants
    {
        XMFLOAT4X4 World;
    };
    vsPerObjectConstants _vsPerObjectConstants;
    ComPtr<ID3D11Buffer> _vsPerObjectConstantBuffer;

    // scratch cache so we don't reallocate each frame
    std::vector<std::shared_ptr<Light>> _cachedLights;
    std::vector<std::shared_ptr<Object>> _cachedObjects;
};

class PointLightShadows
{
public:
    PointLightShadows(ID3D11DeviceContext* context);

    void BuildShadowmaps(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene);

private:
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;
    ComPtr<ID3D11InputLayout> _inputLayout;

    std::shared_ptr<Texture2D> _shadowDepth;
    std::shared_ptr<Texture2D> _blurScratch;
    std::shared_ptr<Blur> _blur;

    struct vsPerLightConstants
    {
        XMFLOAT4X4 View;
        float ParaboloidNear;
        float ParaboloidFar;
        XMFLOAT2 Padding;
    };
    vsPerLightConstants _vsPerLightConstants;
    ComPtr<ID3D11Buffer> _vsPerLightConstantBuffer;

    struct vsPerObjectConstants
    {
        XMFLOAT4X4 World;
    };
    vsPerObjectConstants _vsPerObjectConstants;
    ComPtr<ID3D11Buffer> _vsPerObjectConstantBuffer;

    // scratch cache so we don't reallocate each frame
    std::vector<std::shared_ptr<Light>> _cachedLights;
    std::vector<std::shared_ptr<Object>> _cachedObjects;
};

