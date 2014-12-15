#include "Precomp.h"
#include "VRES.h"
#include "Materials\Diffuse.h"
#include <DirectXTex.h>

using namespace VRES;

std::unique_ptr<Renderer> Renderer::Create(const RendererConfig& config)
{
    std::unique_ptr<Renderer> renderer(new Renderer);
    if (!renderer->Initialize(config))
    {
        return nullptr;
    }

    return renderer;
}

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::ClearBackBuffer(const float clearColor[4])
{
    _context->ClearRenderTargetView(_renderTargetView.Get(), clearColor);
}

void Renderer::ClearDepth(float depth)
{
    _context->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}

bool Renderer::EndFrame()
{
    HRESULT hr = _swapChain->Present(1, 0);
    if (FAILED(hr))
    {
        assert(false);
        return false;
    }

    return true;
}

std::shared_ptr<Model> Renderer::CreateCube(const XMFLOAT3& halfWidths)
{
    VertexStaticMesh vertices[] = 
    {
        { XMFLOAT3(-halfWidths.x, halfWidths.y, -halfWidths.z), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0, 0) },
        { XMFLOAT3(halfWidths.x, halfWidths.y, -halfWidths.z), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1, 0) },
        { XMFLOAT3(halfWidths.x, -halfWidths.y, -halfWidths.z), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1, 1) },
        { XMFLOAT3(-halfWidths.x, -halfWidths.y, -halfWidths.z), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0, 1) },

        { XMFLOAT3(halfWidths.x, halfWidths.y, halfWidths.z), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0, 0) },
        { XMFLOAT3(-halfWidths.x, halfWidths.y, halfWidths.z), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1, 0) },
        { XMFLOAT3(-halfWidths.x, -halfWidths.y, halfWidths.z), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1, 1) },
        { XMFLOAT3(halfWidths.x, -halfWidths.y, halfWidths.z), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0, 1) },

        { XMFLOAT3(-halfWidths.x, halfWidths.y, halfWidths.z), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0, 0) },
        { XMFLOAT3(-halfWidths.x, halfWidths.y, -halfWidths.z), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1, 0) },
        { XMFLOAT3(-halfWidths.x, -halfWidths.y, -halfWidths.z), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1, 1) },
        { XMFLOAT3(-halfWidths.x, -halfWidths.y, halfWidths.z), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0, 1) },

        { XMFLOAT3(halfWidths.x, halfWidths.y, -halfWidths.z), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0, 0) },
        { XMFLOAT3(halfWidths.x, halfWidths.y, halfWidths.z), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1, 0) },
        { XMFLOAT3(halfWidths.x, -halfWidths.y, halfWidths.z), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1, 1) },
        { XMFLOAT3(halfWidths.x, -halfWidths.y, -halfWidths.z), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0, 1) },

        { XMFLOAT3(-halfWidths.x, halfWidths.y, halfWidths.z), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0, 0) },
        { XMFLOAT3(halfWidths.x, halfWidths.y, halfWidths.z), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1, 0) },
        { XMFLOAT3(halfWidths.x, halfWidths.y, -halfWidths.z), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1, 1) },
        { XMFLOAT3(-halfWidths.x, halfWidths.y, -halfWidths.z), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0, 1) },

        { XMFLOAT3(-halfWidths.x, -halfWidths.y, -halfWidths.z), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0, 0) },
        { XMFLOAT3(halfWidths.x, -halfWidths.y, -halfWidths.z), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1, 0) },
        { XMFLOAT3(halfWidths.x, -halfWidths.y, halfWidths.z), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1, 1) },
        { XMFLOAT3(-halfWidths.x, -halfWidths.y, halfWidths.z), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0, 1) },
    };

    uint32_t indices[] =
    {
        0,  1,  2,  0,  2,  3,
        4,  5,  6,  4,  6,  7,
        8,  9,  10, 8,  10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23,
    };

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(vertices);
    bd.StructureByteStride = sizeof(vertices[0]);
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = vertices;
    init.SysMemPitch = sizeof(vertices);
    init.SysMemSlicePitch = init.SysMemPitch;

    ComPtr<ID3D11Buffer> vb;
    HRESULT hr = _device->CreateBuffer(&bd, &init, &vb);
    if (FAILED(hr)) return nullptr;

    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.ByteWidth = sizeof(indices);
    bd.StructureByteStride = sizeof(indices[0]);

    init.pSysMem = indices;
    init.SysMemPitch = sizeof(indices);
    init.SysMemSlicePitch = init.SysMemPitch;

    ComPtr<ID3D11Buffer> ib;
    hr = _device->CreateBuffer(&bd, &init, &ib);
    if (FAILED(hr)) return nullptr;

    return std::shared_ptr<Model>(new Model(VertexFormat::StaticMesh, vb, ib, _countof(vertices), _countof(indices)));
}

_Use_decl_annotations_
std::shared_ptr<Model> Renderer::LoadModel(const wchar_t* filename)
{
    UNREFERENCED_PARAMETER(filename);
    return nullptr;
}

_Use_decl_annotations_
ComPtr<ID3D11ShaderResourceView> Renderer::LoadTexture(const wchar_t* filename)
{
    TexMetadata metadata;
    ScratchImage image;
    HRESULT hr = LoadFromDDSFile(filename, DDS_FLAGS_NONE, &metadata, image);
    if (FAILED(hr))
    {
        hr = LoadFromTGAFile(filename, &metadata, image);
        if (FAILED(hr))
        {
            hr = LoadFromWICFile(filename, WIC_FLAGS_NONE, &metadata, image);
            if (FAILED(hr)) return nullptr;
        }
    }

    ComPtr<ID3D11Resource> resource;
    hr = CreateTexture(_device.Get(), image.GetImages(), image.GetImageCount(), metadata, &resource);
    if (FAILED(hr)) return nullptr;

    ComPtr<ID3D11ShaderResourceView> srv;
    hr = _device->CreateShaderResourceView(resource.Get(), nullptr, &srv);
    if (FAILED(hr)) return nullptr;

    return srv;
}

void Renderer::RenderScene(const std::shared_ptr<Scene>& scene, const Transform& camera,  const std::shared_ptr<HMD>& hmd)
{
    hmd->SetCameraTransform(camera.Position(), camera.Orientation());

    auto models = scene->Models();
    for (uint32_t i = 0; i < scene->NumModels(); ++i)
    {
        auto& material = models[i]->Material();
        if (material != nullptr)
        {
            material->RenderModels(&models[i], 1, scene->Lights(), scene->NumLights(), hmd);
        }
    }
}

void Renderer::AddMaterial(std::shared_ptr<Material>& material)
{
    _materials[material->Name()] = material;
}

std::shared_ptr<Material> Renderer::GetMaterial(const std::string& name)
{
    auto it = _materials.find(name);
    return (it != _materials.end() ? it->second : nullptr);
}

bool Renderer::Initialize(const RendererConfig& config)
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 2;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Width = config.Resolution.Width;
    scd.BufferDesc.Height = config.Resolution.Height;
    scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = (HWND)config.Window;
    scd.SampleDesc.Count = 1;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Windowed = TRUE;

    uint32_t flags = 0;

    if (config.CreateDebugDevice)
    {
        flags |= D3D11_CREATE_DEVICE_DEBUG;
    }

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        flags, &featureLevel, 1, D3D11_SDK_VERSION, &scd, &_swapChain, &_device, nullptr, &_context);
    if (FAILED(hr)) return false;

    ComPtr<ID3D11Texture2D> texture;
    hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(&texture));
    if (FAILED(hr)) return false;

    hr = _device->CreateRenderTargetView(texture.Get(), nullptr, &_renderTargetView);
    if (FAILED(hr)) return false;

    D3D11_TEXTURE2D_DESC td;
    texture->GetDesc(&td);

    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.Format = DXGI_FORMAT_D32_FLOAT;
    td.SampleDesc.Count = 1;

    hr = _device->CreateTexture2D(&td, nullptr, &texture);
    if (FAILED(hr)) return false;

    hr = _device->CreateDepthStencilView(texture.Get(), nullptr, &_depthStencilView);
    if (FAILED(hr)) return false;

    _context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());

    //
    // Create built-in objects
    //
    D3D11_SAMPLER_DESC sd = {};
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    hr = _device->CreateSamplerState(&sd, &_linearWrapSampler);
    if (FAILED(hr)) return false;

    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

    hr = _device->CreateSamplerState(&sd, &_pointClampSampler);
    if (FAILED(hr)) return false;

    //
    // Create built-in materials
    //
    std::shared_ptr<Material> material = DiffuseMaterial::Create(this, _context);
    if (!material) return false;

    AddMaterial(material);

    return true;
}
