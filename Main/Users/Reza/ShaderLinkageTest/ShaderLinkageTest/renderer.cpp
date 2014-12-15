#include "precomp.h"
#include "renderer.h"
#include "camera.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace DirectX;

//==============================================================================

_Use_decl_annotations_
Renderer::Renderer(HWND target, uint32_t backBufferWidth, uint32_t backBufferHeight) :
    _camera(std::make_shared<Camera>()), _wallsIndexCount(0), _wallsIndexOffset(0), _cubeIndexOffset(0), _cubeIndexCount(0)
{
    assert(target != nullptr);
    CreateDeviceResources(target, backBufferWidth, backBufferHeight);
    LoadShaders();

    LoadScene();

    BindRenderPipeline();
}

//==============================================================================

void Renderer::Render()
{
    BindRenderPipeline();

    Clear();

    // Update shader params
    static float rotation = 0.0f;
    static LARGE_INTEGER start = {};
    static LARGE_INTEGER freq = {};
    static bool init = false;

    if (!init)
    {
        QueryPerformanceCounter(&start);
        QueryPerformanceFrequency(&freq);
        init = true;
    }

    LARGE_INTEGER now = {};
    QueryPerformanceCounter(&now);

    float deltaSeconds = (now.QuadPart - start.QuadPart) / (float)freq.QuadPart;
    start = now;
    rotation += deltaSeconds;

    XMStoreFloat4x4(&_cubeWorld, 
        XMMatrixMultiply(
            XMMatrixMultiply(XMMatrixRotationY(rotation), XMMatrixScaling(3.0f, 3.0f, 3.0f)),
                XMMatrixTranslation(0, 0, 0)));

    XMMATRIX view = XMLoadFloat4x4(&_camera->GetView());
    XMMATRIX proj = XMLoadFloat4x4(&_camera->GetProjection());
    XMStoreFloat4x4(&_vertexShaderConstants.ViewProjection, XMMatrixMultiply(view, proj));
    _context->UpdateSubresource(_vertexShaderConstantBuffer.Get(), 0, nullptr, &_vertexShaderConstants, sizeof(_vertexShaderConstants), 0);

    DrawScene();

    Present();
}

//==============================================================================

_Use_decl_annotations_
void Renderer::CreateDeviceResources(HWND target, uint32_t backBufferWidth, uint32_t backBufferHeight)
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 2;
    scd.BufferDesc.Width = backBufferWidth;
    scd.BufferDesc.Height = backBufferHeight;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 1;
    scd.BufferDesc.RefreshRate.Denominator = 60;
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
    scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = target;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    scd.Windowed = TRUE;

    UINT deviceFlags = 0;

#ifdef _DEBUG
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL requestedFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags,
        requestedFeatureLevels, _countof(requestedFeatureLevels), D3D11_SDK_VERSION, &scd, &_swapChain,
        &_device, &featureLevel, &_context);
    if (FAILED(hr))
    {
        throw std::exception("D3D11CreateDeviceAndSwapChain failed!");
    }

    ComPtr<ID3D11Texture2D> texture;
    hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(&texture));
    if (FAILED(hr))
    {
        throw std::exception("SwapChain::GetBuffer failed!");
    }

    hr = _device->CreateRenderTargetView(texture.Get(), nullptr, &_backBuffer);
    if (FAILED(hr))
    {
        throw std::exception("CreateRenderTargetView failed!");
    }

    D3D11_TEXTURE2D_DESC td = {};
    td.ArraySize = 1;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    td.Width = backBufferWidth;
    td.Height = backBufferHeight;
    td.MipLevels = 1;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;

    hr = _device->CreateTexture2D(&td, nullptr, &texture);
    if (FAILED(hr))
    {
        throw std::exception("CreateRenderTargetView failed!");
    }

    hr = _device->CreateDepthStencilView(texture.Get(), nullptr, &_depthStencil);
    if (FAILED(hr))
    {
        throw std::exception("CreateRenderTargetView failed!");
    }

    D3D11_VIEWPORT vp = {};
    vp.Width = static_cast<float>(backBufferWidth);
    vp.Height = static_cast<float>(backBufferHeight);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

    _context->RSSetViewports(1, &vp);

    D3D11_SAMPLER_DESC sd = {};
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    hr = _device->CreateSamplerState(&sd, &_linearSampler);
    if (FAILED(hr))
    {
        throw std::exception();
    }
}

std::unique_ptr<BYTE[]> Renderer::LoadShader(const wchar_t* filename, _Out_ DWORD* numBytes)
{
    FileHandle file(CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file.IsValid())
    {
        throw std::exception();
    }

    *numBytes = GetFileSize(file.Get(), nullptr);
    std::unique_ptr<BYTE[]> buffer(new BYTE[*numBytes]);

    DWORD bytesRead = 0;
    if (!ReadFile(file.Get(), buffer.get(), *numBytes, &bytesRead, nullptr) || bytesRead != *numBytes)
    {
        throw std::exception();
    }

    return buffer;
}

void Renderer::LoadShaders()
{
    // Vertex Shader
    DWORD vertexShaderLength = 0;
    auto vertexShaderBytes = LoadShader(L"VertexShader.cso", &vertexShaderLength);
    HRESULT hr = _device->CreateVertexShader(vertexShaderBytes.get(), vertexShaderLength, nullptr, &_vertexShader);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    // Pixel Shader
    DWORD pixelShaderLength = 0;
    auto pixelShaderBytes = LoadShader(L"PixelShader.cso", &pixelShaderLength);
    hr = _device->CreatePixelShader(pixelShaderBytes.get(), pixelShaderLength, nullptr, &_pixelShader);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    // Input Layout
    D3D11_INPUT_ELEMENT_DESC elems[2] = {};
    elems[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    elems[0].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
    elems[0].SemanticName = "POSITION";
    elems[1].AlignedByteOffset = sizeof(XMFLOAT3);
    elems[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    elems[1].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
    elems[1].SemanticName = "NORMAL";

    hr = _device->CreateInputLayout(elems, _countof(elems), vertexShaderBytes.get(), vertexShaderLength, &_inputLayout);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    // Constant Buffers
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = bd.StructureByteStride = sizeof(_vertexShaderConstants);
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = &_vertexShaderConstants;
    init.SysMemPitch = bd.ByteWidth;
    init.SysMemSlicePitch = 0;

    hr = _device->CreateBuffer(&bd, &init, &_vertexShaderConstantBuffer);
    if (FAILED(hr))
    {
        throw std::exception();
    }
}


void Renderer::Clear()
{
    static const float clearColor[] = { 0.0f, 0.0f, 0.5f, 1.0f };
    _context->ClearRenderTargetView(_backBuffer.Get(), clearColor);
    _context->ClearDepthStencilView(_depthStencil.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    _context->OMSetRenderTargets(1, _backBuffer.GetAddressOf(), _depthStencil.Get());
}

void Renderer::Present()
{
    _swapChain->Present(1, 0);
}
