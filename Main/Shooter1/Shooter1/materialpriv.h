#pragma once

//
// Base Implementation class for materials
//
class Material
{
public:
    virtual ~Material()
    {}

    MaterialType GetType() const { return _type; }

    // Configure graphics pipeline state
    void ApplyMaterial();

    virtual void Draw(_In_ const XMFLOAT4X4& view, _In_ const XMFLOAT4X4& projection, _In_count_(numObjects) GameObject** objects, _In_ uint32_t numObjects) = 0;

protected:
    Material(_In_ MaterialType type) : _context(GetGraphics().GetContext()), _type(type)
    {}

    const ComPtr<ID3D11DeviceContext>& GetContext() const { return _context; }

    //
    // Configuration
    //
    void SetVertexShader(_In_z_ const char* filename, _In_ VertexFormat format);
    void SetPixelShader(_In_z_ const char* filename);

private:
    Material(const Material&);

private:
    MaterialType _type;
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<ID3D11InputLayout> _inputLayout;
    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;
    ComPtr<ID3D11RasterizerState> _rasterizer;
    ComPtr<ID3D11BlendState> _blendState;
    ComPtr<ID3D11DepthStencilState> _depthState;
};
