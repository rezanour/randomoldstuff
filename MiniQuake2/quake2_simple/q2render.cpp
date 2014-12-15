#include "stdafx.h"
#include "q2render.h"
#include "q2renderp.h"
#include "q2bspp.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace GDK;

class Renderer
{
public:
    Renderer(
        HWND hwnd,
        int screenWidth,
        int screenHeight
        );

    ~Renderer();

    void
    Render(
        const Vector3& playerPosition,
        float playerAngle
        );

    uint32_t
    RegisterTexture(
        _In_ PCSTR texture
        );

    void
    ClearTextures();

    void
    DrawTriangle(
        _In_ uint32_t texture,
        _In_ const GDK::Vector3& v0,
        _In_ const GDK::Vector2& tex0,
        _In_ const GDK::Vector3& v1,
        _In_ const GDK::Vector2& tex1,
        _In_ const GDK::Vector3& v2,
        _In_ const GDK::Vector2& tex2
        );

private:
    void
    DrawRenderInfos();

private:
    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<IDXGISwapChain> _swapChain;
    ComPtr<ID3D11RenderTargetView> _backBuffer;
    ComPtr<ID3D11DepthStencilView> _depthBuffer;

    ComPtr<ID3D11VertexShader> _vertexShader;
    ComPtr<ID3D11PixelShader> _pixelShader;
    ComPtr<ID3D11Buffer> _vsConstants;
    ComPtr<ID3D11Buffer> _psConstants;
    ComPtr<ID3D11Buffer> _vertexBuffer;
    ComPtr<ID3D11InputLayout> _inputLayout;
    ComPtr<ID3D11SamplerState> _sampler;

    struct VSGlobals {
        Matrix World;
        Matrix InvTransWorld;
        Matrix ViewProjection;
    };

    struct DirectionalLight {
        Vector4 Normal;
        Vector4 Color;
    };

    struct PSGlobals {
        DirectionalLight Light1;
        DirectionalLight Light2;
        DirectionalLight Light3;
    };

    VSGlobals _vsGlobals;
    PSGlobals _psGlobals;

    struct Vertex {
        Vector3 Position;
        Vector3 Normal;
        Vector2 UV;
    };

    static const uint32_t MaxVerticesPerDraw = 8000;

    struct RenderInfo {
        std::string TextureName;
        uint32_t TextureWidth;
        uint32_t TextureHeight;

        ComPtr<ID3D11Texture2D> Texture;
        ComPtr<ID3D11ShaderResourceView> TextureSRV;

        Vertex Vertices[MaxVerticesPerDraw];
        uint32_t NumVertices;
    };

    std::vector<std::unique_ptr<RenderInfo>> _renderInfos;

    Matrix _view;
    Matrix _projection;
};

static Renderer* g_renderer = nullptr;

void
RenderInit(
    HWND hwnd,
    int screenWidth,
    int screenHeight
    )
{
    if (g_renderer != nullptr)
    {
        RIP(L"Renderer already initialized.\n");
    }

    g_renderer = new Renderer(hwnd, screenWidth, screenHeight);
}

void
RenderShutdown()
{
    if (g_renderer != nullptr)
    {
        delete g_renderer;
        g_renderer = nullptr;
    }
}

void
Render(
    const Vector3& playerPosition,
    float playerAngle
    )
{
    if (g_renderer == nullptr)
    {
        RIP(L"Renderer not initialized.\n");
    }

    g_renderer->Render(playerPosition, playerAngle);
}

/*******************************************************************************/

uint32_t
RenderpRegisterTexture(
    _In_ PCSTR texture
    )
{
    if (g_renderer == nullptr)
    {
        RIP(L"Renderer not initialized.\n");
    }

    g_renderer->RegisterTexture(texture);
}

void
RenderpClearTextures()
{
    // might be called before renderer is there
    if (g_renderer != nullptr)
    {
        g_renderer->ClearTextures();
    }
}

void
RenderpDrawTriangle(
    _In_ uint32_t texture,
    _In_ const GDK::Vector3& v0,
    _In_ const GDK::Vector2& tex0,
    _In_ const GDK::Vector3& v1,
    _In_ const GDK::Vector2& tex1,
    _In_ const GDK::Vector3& v2,
    _In_ const GDK::Vector2& tex2
    )
{
    if (g_renderer == nullptr)
    {
        RIP(L"Renderer not initialized.\n");
    }

    g_renderer->DrawTriangle(texture, v0, tex0, v1, tex1, v2, tex2);
}

/*******************************************************************************/

Renderer::Renderer(
    HWND hwnd,
    int screenWidth,
    int screenHeight
    )
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.OutputWindow = hwnd;
    scd.BufferCount = 3;
    scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferDesc.Width = screenWidth;
    scd.BufferDesc.Height = screenHeight;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Windowed = TRUE;

    D3D_FEATURE_LEVEL featureLevels[] = { 
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    D3D_FEATURE_LEVEL featureLevel;

    UINT flags = 0;

#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        &scd,
        &_swapChain,
        &_device,
        &featureLevel,
        &_context);
    if (FAILED(hr))
    {
        RIP(L"Failed to create D3D device & swapchain. 0x%08.x\n", hr);
    }

    ComPtr<ID3D11Texture2D> texture;
    hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(&texture));
    if (FAILED(hr))
    {
        RIP(L"Failed to get back buffer surface from swapchain. 0x%08x.\n", hr);
    }

    hr = _device->CreateRenderTargetView(texture.Get(), nullptr, &_backBuffer);
    if (FAILED(hr))
    {
        RIP(L"Failed to create render target view for back buffer. 0x%08x.\n", hr);
    }

    D3D11_TEXTURE2D_DESC td = {};
    texture->GetDesc(&td);
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.Format = DXGI_FORMAT_D32_FLOAT;

    hr = _device->CreateTexture2D(&td, nullptr, texture.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        RIP(L"Failed to create depth buffer. 0x%08x.\n", hr);
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};
    dsv.Format = DXGI_FORMAT_D32_FLOAT;
    dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsv.Texture2D.MipSlice = 0;
    hr = _device->CreateDepthStencilView(texture.Get(), &dsv, &_depthBuffer);
    if (FAILED(hr))
    {
        RIP(L"Failed to create depth stencil view for depth buffer. 0x%08x.\n", hr);
    }

    _context->OMSetRenderTargets(1, _backBuffer.GetAddressOf(), _depthBuffer.Get());

    FileHandle vs(CreateFile(L"q2vertexshader.cso", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!vs.IsValid())
    {
        RIP(L"Failed to open vertex shader file. %d\n", GetLastError());
    }

    DWORD bufferLength = GetFileSize(vs.Get(), nullptr);
    unsigned char* buffer = new unsigned char[bufferLength];

    DWORD bytesRead = 0;
    if (!ReadFile(vs.Get(), buffer, bufferLength, &bytesRead, nullptr))
    {
        RIP(L"Failed to read vs buffer. %d\n", GetLastError());
    }

    hr = _device->CreateVertexShader(buffer, bufferLength, nullptr, &_vertexShader);
    if (FAILED(hr))
    {
        RIP(L"Failed to create vertex shader. 0x%08x\n", hr);
    }

    D3D11_INPUT_ELEMENT_DESC elems[3] = {};
    elems[0].SemanticName = "POSITION";
    elems[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    elems[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    elems[1].AlignedByteOffset = 12;
    elems[1].SemanticName = "NORMAL";
    elems[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    elems[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    elems[2].AlignedByteOffset = 24;
    elems[2].SemanticName = "TEXCOORD";
    elems[2].Format = DXGI_FORMAT_R32G32_FLOAT;
    elems[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    hr = _device->CreateInputLayout(elems, _countof(elems), buffer, bufferLength, &_inputLayout);
    if (FAILED(hr))
    {
        RIP(L"Failed to create input layout. 0x%08x\n", hr);
    }

    FileHandle ps(CreateFile(L"q2pixelshader.cso", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!ps.IsValid())
    {
        RIP(L"Failed to open pixel shader file. %d\n", GetLastError());
    }

    bufferLength = GetFileSize(ps.Get(), nullptr);
    delete [] buffer;
    buffer = new unsigned char[bufferLength];

    if (!ReadFile(ps.Get(), buffer, bufferLength, &bytesRead, nullptr))
    {
        RIP(L"Failed to read ps buffer. %d\n", GetLastError());
    }

    hr = _device->CreatePixelShader(buffer, bufferLength, nullptr, &_pixelShader);
    if (FAILED(hr))
    {
        RIP(L"Failed to create pixel shader. 0x%08x\n", hr);
    }

    _view = Matrix::CreateLookAt(Vector3::Zero(), Vector3::Forward(), Vector3::Up());
    _projection = Matrix::CreatePerspectiveFov(3.14156f / 4, screenWidth / (float)screenHeight, 0.01f, 10000.0f);

    _vsGlobals.World = Matrix::Identity();
    _vsGlobals.InvTransWorld = Matrix::Identity();
    _vsGlobals.ViewProjection = _view * _projection;

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.StructureByteStride = sizeof(VSGlobals);
    bd.ByteWidth = bd.StructureByteStride;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.SysMemPitch = bd.ByteWidth;
    initData.SysMemSlicePitch = 0;
    initData.pSysMem = &_vsGlobals;

    hr = _device->CreateBuffer(&bd, &initData, &_vsConstants);
    if (FAILED(hr))
    {
        RIP(L"Failed to create vs globals constant buffer. 0x%08x\n", hr);
    }

    _psGlobals.Light1.Normal = Vector4::UnitY();
    _psGlobals.Light1.Color = Vector4::One();

    _psGlobals.Light2.Normal = Vector4::UnitX();
    _psGlobals.Light2.Color = Vector4(0.2f, 0.2f, 0.2f, 1.0f);

    _psGlobals.Light3.Normal = Vector4::UnitZ();
    _psGlobals.Light3.Color = Vector4(0.4f, 0.4f, 0.4f, 1.0f);

    bd.StructureByteStride = sizeof(PSGlobals);
    bd.ByteWidth = bd.StructureByteStride;

    initData.SysMemPitch = bd.ByteWidth;
    initData.pSysMem = &_psGlobals;

    hr = _device->CreateBuffer(&bd, &initData, &_psConstants);
    if (FAILED(hr))
    {
        RIP(L"Failed to create ps globals constant buffer. 0x%08x\n", hr);
    }

    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->VSSetConstantBuffers(0, 1, _vsConstants.GetAddressOf());

    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);
    _context->PSSetConstantBuffers(0, 1, _psConstants.GetAddressOf());

    _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _context->IASetInputLayout(_inputLayout.Get());

    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.StructureByteStride = sizeof(Vertex);
    bd.ByteWidth = MaxVerticesPerDraw * bd.StructureByteStride;

    std::unique_ptr<Vertex> dummy(new Vertex[MaxVerticesPerDraw]);
    initData.SysMemPitch = bd.ByteWidth;
    initData.pSysMem = dummy.get();

    hr = _device->CreateBuffer(&bd, &initData, &_vertexBuffer);
    if (FAILED(hr))
    {
        RIP(L"Failed to create vertex buffer. 0x%08x\n", hr);
    }

    UINT32 strides[] = { sizeof(Vertex) };
    UINT32 offsets[] = { 0 };
    _context->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), strides, offsets);

    D3D11_SAMPLER_DESC sd = {};
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    hr = _device->CreateSamplerState(&sd, &_sampler);
    if (FAILED(hr))
    {
        RIP(L"Failed to create texture sampler. 0x%08x\n", hr);
    }

    _context->PSSetSamplers(0, 1, _sampler.GetAddressOf());

    D3D11_VIEWPORT vp = {};
    vp.Width = screenWidth;
    vp.Height = screenHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    _context->RSSetViewports(1, &vp);
}

Renderer::~Renderer()
{
}

void
Renderer::Render(
    const Vector3& playerPosition,
    float playerAngle
    )
{
    static const float ClearColor[] = { 0.0f, 0.0f, 0.5f, 1.0f };

    _context->ClearDepthStencilView(_depthBuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    _context->ClearRenderTargetView(_backBuffer.Get(), ClearColor);

    Matrix rot(Matrix::CreateRotationY(playerAngle));
    _view = Matrix::CreateLookAt(playerPosition, playerPosition + rot.GetForward(), Vector3::Up());
    _vsGlobals.ViewProjection = _view * _projection;

    _context->UpdateSubresource(_vsConstants.Get(), 0, nullptr, &_vsGlobals, sizeof(_vsGlobals), 0);

    BsppDrawFrontToBack(playerPosition, playerAngle);

    DrawRenderInfos();

    _swapChain->Present(1, 0);
}

uint32_t
Renderer::RegisterTexture(
    _In_ PCSTR texture
    )
{
    // see if we already have it (yes, this is a crappy linear search...)
    const std::unique_ptr<RenderInfo>* entry = _renderInfos.data();
    uint32_t count = _renderInfos.size();
    while (count-- > 0)
    {
        if (entry->get()->TextureName.compare(texture) == 0)
        {
            return (entry - _renderInfos.data());
        }

        ++entry;
    }

    // Insert new one

    // Create the texture
    uint32_t width, height;
    WalLoadImage(texture, &width, &height);

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.MipLevels = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.ArraySize = 1;

    std::unique_ptr<uint8_t> pixels(new uint8_t[width * height * 4]);
    WalCopyPixels(texture, pixels.get());

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = pixels.get();
    initData.SysMemPitch = width * 4;
    initData.SysMemSlicePitch = initData.SysMemPitch * height;

    std::unique_ptr<RenderInfo> renderInfo(new RenderInfo);
    renderInfo->NumVertices = 0;
    renderInfo->TextureName = texture;
    renderInfo->TextureWidth = width;
    renderInfo->TextureHeight = height;

    HRESULT hr = _device->CreateTexture2D(&texDesc, &initData, &renderInfo->Texture);
    if (FAILED(hr))
    {
        RIP(L"Failed to create texture. 0x%08x\n", hr);
    }

    hr = _device->CreateShaderResourceView(renderInfo->Texture.Get(), nullptr, &renderInfo->TextureSRV);
    if (FAILED(hr))
    {
        RIP(L"Failed to create texture shader resource. 0x%08x\n", hr);
    }

    _renderInfos.push_back(std::move(renderInfo));

    return _renderInfos.size() - 1;
}

void
Renderer::ClearTextures()
{
    _renderInfos.clear();
}

void
Renderer::DrawTriangle(
    _In_ uint32_t texture,
    _In_ const GDK::Vector3& v0,
    _In_ const GDK::Vector2& tex0,
    _In_ const GDK::Vector3& v1,
    _In_ const GDK::Vector2& tex1,
    _In_ const GDK::Vector3& v2,
    _In_ const GDK::Vector2& tex2
    )
{
    if (texture >= _renderInfos.size())
    {
        RIP(L"Invalid texture index\n");
    }

    RenderInfo* renderInfo = _renderInfos[texture].get();

    if (renderInfo->NumVertices + 3 < MaxVerticesPerDraw)
    {
        renderInfo->Vertices[renderInfo->NumVertices].Position = v0;
        renderInfo->Vertices[renderInfo->NumVertices++].UV = GDK::Vector2(tex0.x / renderInfo->TextureWidth, tex0.y / renderInfo->TextureHeight);
        renderInfo->Vertices[renderInfo->NumVertices].Position = v1;
        renderInfo->Vertices[renderInfo->NumVertices++].UV = GDK::Vector2(tex1.x / renderInfo->TextureWidth, tex1.y / renderInfo->TextureHeight);
        renderInfo->Vertices[renderInfo->NumVertices].Position = v2;
        renderInfo->Vertices[renderInfo->NumVertices++].UV = GDK::Vector2(tex2.x / renderInfo->TextureWidth, tex2.y / renderInfo->TextureHeight);
    }
}

void
Renderer::DrawRenderInfos()
{
    std::unique_ptr<RenderInfo>* renderInfoPtr = _renderInfos.data();
    uint32_t count = _renderInfos.size();
    while (count-- > 0)
    {
        RenderInfo* renderInfo = renderInfoPtr->get();

        _context->PSSetShaderResources(0, 1, renderInfo->TextureSRV.GetAddressOf());

        _context->UpdateSubresource(_vertexBuffer.Get(), 0, nullptr, renderInfo->Vertices, sizeof(Vertex) * renderInfo->NumVertices, 0);

        _context->Draw(renderInfo->NumVertices, 0);

        renderInfo->NumVertices = 0;

        ++renderInfoPtr;
    }
}

