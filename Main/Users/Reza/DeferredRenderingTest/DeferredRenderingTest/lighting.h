#pragma once

class Texture2D;
class Object;
class Camera;
class Scene;

enum class LightType
{
    Directional,
    Point
};

class Light : public std::enable_shared_from_this<Light>
{
public:
    Light(LightType type, const XMFLOAT3& color) :
        Type(type), Color(color)
    {
    }

    LightType Type;
    XMFLOAT3 Color;
};

class DirectionalLight : public Light
{
public:
    DirectionalLight(const XMFLOAT3& direction, const XMFLOAT3& color) :
        Light(LightType::Directional, color)
    {
        XMVECTOR dir = XMLoadFloat3(&direction);
        XMStoreFloat3(&Direction, XMVector3Normalize(dir));
    }

    XMFLOAT3 Direction;
    std::shared_ptr<Texture2D> ShadowMap;
    XMFLOAT4X4 View;
    XMFLOAT4X4 Projection;
};

class PointLight : public Light
{
public:
    PointLight(const XMFLOAT3& position, float radius, const XMFLOAT3& color) :
        Light(LightType::Point, color), Position(position), Radius(radius)
    {
    }

    XMFLOAT3 Position;
    float Radius;

    std::shared_ptr<Texture2D> ShadowMapFront;
    std::shared_ptr<Texture2D> ShadowMapBack;
};

class DirectionalLighting
{
public:
    DirectionalLighting(ID3D11DeviceContext* context, const std::shared_ptr<Texture2D>& depth, const std::shared_ptr<Texture2D>& normals, const std::shared_ptr<Texture2D>& lights);

    void RenderFrame(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene);

private:
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;
    ComPtr<ID3D11SamplerState> _pointSampler;
    ComPtr<ID3D11SamplerState> _linearSampler;
    ComPtr<ID3D11BlendState> _additiveBlend;

    // Full screen quad
    std::shared_ptr<Object> _quad;
    ComPtr<ID3D11InputLayout> _inputLayout;

    std::shared_ptr<Texture2D> _depth;
    std::shared_ptr<Texture2D> _normals;
    std::shared_ptr<Texture2D> _lights;

    struct vsPerFrameConstants
    {
        XMFLOAT4X4 InvProjection;
    };
    vsPerFrameConstants _vsPerFrameConstants;
    ComPtr<ID3D11Buffer> _vsPerFrameConstantBuffer;

    struct psPerFrameConstants
    {
        float ProjectionA;
        float ProjectionB;
        XMFLOAT2 ScreenSize;
    };
    psPerFrameConstants _psPerFrameConstants;
    ComPtr<ID3D11Buffer> _psPerFrameConstantBuffer;

    struct psPerLightConstants
    {
        XMFLOAT4 Direction;
        XMFLOAT4 Color;
        XMFLOAT4X4 InvView;
        XMFLOAT4X4 LightView;
        XMFLOAT4X4 LightProjection;
    };
    psPerLightConstants _psPerLightConstants;
    ComPtr<ID3D11Buffer> _psPerLightConstantBuffer;

    // scratch cache so we don't reallocate each frame
    std::vector<std::shared_ptr<Light>> _cachedLights;
};

class PointLighting
{
public:
    PointLighting(ID3D11DeviceContext* context, const std::shared_ptr<Texture2D>& depth, const std::shared_ptr<Texture2D>& normals, const std::shared_ptr<Texture2D>& lights);

    void RenderFrame(const std::shared_ptr<Camera>& camera, const std::shared_ptr<Scene>& scene);

private:
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;
    ComPtr<ID3D11SamplerState> _pointSampler;
    ComPtr<ID3D11SamplerState> _linearSampler;
    ComPtr<ID3D11BlendState> _additiveBlend;

    // Full screen quad
    std::shared_ptr<Object> _quad;
    ComPtr<ID3D11InputLayout> _inputLayout;

    std::shared_ptr<Texture2D> _depth;
    std::shared_ptr<Texture2D> _normals;
    std::shared_ptr<Texture2D> _lights;

    struct vsPerFrameConstants
    {
        XMFLOAT4X4 InvProjection;
        XMFLOAT4X4 View;
        XMFLOAT4X4 Projection;
    };
    vsPerFrameConstants _vsPerFrameConstants;
    ComPtr<ID3D11Buffer> _vsPerFrameConstantBuffer;

    struct vsPerObjectConstants
    {
        XMFLOAT4X4 World;
    };
    vsPerObjectConstants _vsPerObjectConstants;
    ComPtr<ID3D11Buffer> _vsPerObjectConstantBuffer;

    struct psPerFrameConstants
    {
        float ProjectionA;
        float ProjectionB;
        XMFLOAT2 ScreenSize;
    };
    psPerFrameConstants _psPerFrameConstants;
    ComPtr<ID3D11Buffer> _psPerFrameConstantBuffer;

    struct psPerLightConstants
    {
        XMFLOAT4X4 InvView;
        XMFLOAT4 Position;
        XMFLOAT4 Color;
        float Radius;
        float ParaboloidNear;
        XMFLOAT2 Padding;
    };
    psPerLightConstants _psPerLightConstants;
    ComPtr<ID3D11Buffer> _psPerLightConstantBuffer;

    // scratch cache so we don't reallocate each frame
    std::vector<std::shared_ptr<Light>> _cachedLights;
};
