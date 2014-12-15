#include "Precomp.h"
#include <wrl\wrappers\corewrappers.h>

using Microsoft::WRL::ComPtr;
namespace Mwrlw = Microsoft::WRL::Wrappers;

namespace GDK {
namespace Graphics {

const wchar_t* const DxGraphicsDevice::DisplayName = L"DirectX 11 Renderer";

// IGraphicsDeviceFactory
HRESULT GDKAPI DxGraphicsDeviceFactory::CreateGraphicsDevice(_In_ const GraphicsDeviceCreationParameters& parameters, _COM_Outptr_ IGraphicsDevice** device)
{
    MODULE_GUARD_BEGIN

    CHECKHR(DxGraphicsDevice::Create(parameters).CopyTo(device));

    MODULE_GUARD_END
}

DxGraphicsDevice::DxGraphicsDevice(_In_ const GraphicsDeviceCreationParameters& parameters) :
    _parameters(parameters),
    _featureLevel(D3D_FEATURE_LEVEL_10_0)
{
}

DxGraphicsDevice::~DxGraphicsDevice()
{
}

ComPtr<DxGraphicsDevice> DxGraphicsDevice::Create(_In_ const GraphicsDeviceCreationParameters& parameters)
{
    ComPtr<DxGraphicsDevice> device = Make<DxGraphicsDevice>(parameters);

    DXGI_SWAP_CHAIN_DESC scd = {};

    scd.OutputWindow = static_cast<HWND>(parameters.windowIdentity);
    scd.Windowed = parameters.scaleMode != GraphicsDeviceScaleMode::FullScreen;
    scd.BufferCount = scd.Windowed ? 1 : 2;
    scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Width = static_cast<uint32_t>(parameters.backBufferWidth);
    scd.BufferDesc.Height = static_cast<uint32_t>(parameters.backBufferHeight);
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    DWORD flags = 0;

#ifndef NDEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    CHECKHR(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        &scd,
        &device->_swapChain,
        &device->_device,
        &device->_featureLevel,
        &device->_context));

    ComPtr<ID3D11Texture2D> texture;

    // Get backbuffer
    CHECKHR(device->_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &texture));
    CHECKHR(device->_device->CreateRenderTargetView(texture.Get(), nullptr, &device->_backBuffer));

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<float>(parameters.backBufferWidth);
    vp.Height = static_cast<float>(parameters.backBufferHeight);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    device->_context->RSSetViewports(1, &vp);

    D3D11_TEXTURE2D_DESC texDesc = {};
    D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {};

    // Create our depth & stencil buffer
    texDesc.Format = depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    texDesc.Width = static_cast<uint32_t>(parameters.backBufferWidth);
    texDesc.Height = static_cast<uint32_t>(parameters.backBufferHeight);
    texDesc.ArraySize = 1;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    texDesc.MipLevels = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;

    depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    CHECKHR(device->_device->CreateTexture2D(&texDesc, nullptr, texture.ReleaseAndGetAddressOf()));
    CHECKHR(device->_device->CreateDepthStencilView(texture.Get(), &depthDesc, &device->_depthStencil));

    // Load precompiled shaders
    Mwrlw::FileHandle shaderFile(CreateFile(L"StaticGeometryVS.cso", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    CHECK_IS_TRUE(shaderFile.IsValid(), HRESULT_FROM_WIN32(GetLastError()));

    BY_HANDLE_FILE_INFORMATION info = {0};
    CHECK_IS_TRUE(GetFileInformationByHandle(shaderFile.Get(), &info), HRESULT_FROM_WIN32(GetLastError()));

    // don't support really huge files (no shader should ever be that big...)
    CHECK_IS_TRUE(info.nFileSizeHigh == 0, HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED));

    UINT fileLength = info.nFileSizeLow;
    std::unique_ptr<byte[]> shaderByteCode(new byte[fileLength]);
    DWORD bytesRead = 0;
    CHECK_IS_TRUE(ReadFile(shaderFile.Get(), shaderByteCode.get(), fileLength, &bytesRead, nullptr), HRESULT_FROM_WIN32(GetLastError()));

    CHECKHR(device->GetDevice()->CreateVertexShader(shaderByteCode.get(), fileLength, nullptr, &device->_vertexShader));

    std::vector<D3D11_INPUT_ELEMENT_DESC> elems;

    D3D11_INPUT_ELEMENT_DESC elem = {0};
    elem.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

    elem.SemanticName = "POSITION";
    elem.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    elems.push_back(elem);

    elem.SemanticName = "NORMAL";
    elem.AlignedByteOffset = 12;
    elems.push_back(elem);

    elem.SemanticName = "TEXCOORD";
    elem.Format = DXGI_FORMAT_R32G32_FLOAT;
    elem.AlignedByteOffset = 24;
    elems.push_back(elem);

    CHECKHR(device->GetDevice()->CreateInputLayout(elems.data(), static_cast<UINT>(elems.size()), shaderByteCode.get(), fileLength, &device->_inputLayout));

    shaderFile.Close();
    shaderFile.Attach(CreateFile(L"TexturedPS.cso", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    CHECK_IS_TRUE(shaderFile.IsValid(), HRESULT_FROM_WIN32(GetLastError()));

    CHECK_IS_TRUE(GetFileInformationByHandle(shaderFile.Get(), &info), HRESULT_FROM_WIN32(GetLastError()));

    fileLength = (static_cast<uint64_t>(info.nFileSizeHigh) << 32) | static_cast<uint64_t>(info.nFileSizeLow);
    shaderByteCode.reset(new byte[fileLength]);

    CHECK_IS_TRUE(ReadFile(shaderFile.Get(), shaderByteCode.get(), fileLength, &bytesRead, nullptr), HRESULT_FROM_WIN32(GetLastError()));

    CHECKHR(device->GetDevice()->CreatePixelShader(shaderByteCode.get(), fileLength, nullptr, &device->_pixelShader));

    D3D11_BUFFER_DESC desc = {0};

    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.ByteWidth = static_cast<UINT>(sizeof(device->_vsConstants));
    desc.StructureByteStride = static_cast<UINT>(sizeof(device->_vsConstants));
    desc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {0};
    init.pSysMem = &device->_vsConstants;
    init.SysMemPitch = desc.ByteWidth;
    init.SysMemSlicePitch = 0;

    CHECKHR(device->GetDevice()->CreateBuffer(&desc, &init, &device->_vsConstantBuffer));

    desc.ByteWidth = static_cast<UINT>(sizeof(device->_psConstants));
    desc.StructureByteStride = static_cast<UINT>(sizeof(device->_psConstants));

    init.pSysMem = &device->_psConstants;
    init.SysMemPitch = desc.ByteWidth;

    CHECKHR(device->GetDevice()->CreateBuffer(&desc, &init, &device->_psConstantBuffer));

    D3D11_RASTERIZER_DESC rsDesc;
    ZeroMemory(&desc, sizeof(desc));
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.DepthClipEnable = TRUE;
    rsDesc.FillMode = D3D11_FILL_WIREFRAME;
    CHECKHR(device->GetDevice()->CreateRasterizerState(&rsDesc, &device->_wireframeRasterizerState));

    device->GetContext()->VSSetShader(device->_vertexShader.Get(), nullptr, 0);
    device->GetContext()->PSSetShader(device->_pixelShader.Get(), nullptr, 0);
    device->GetContext()->IASetInputLayout(device->_inputLayout.Get());
    device->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    device->GetContext()->OMSetRenderTargets(1, device->_backBuffer.GetAddressOf(), device->_depthStencil.Get());

    // Light1
    device->_psConstants.Light1.Direction = Vector4(0, 0, -1, 1);
    device->_psConstants.Light1.Color = Vector4(1, 1, 1, 1);

    // Light2
    device->_psConstants.Light2.Direction = Vector4(0, 1, 0, 1);
    device->_psConstants.Light2.Color = Vector4(0, 0, 1, 1);

    device->UpdatePSConstantBuffer();

    // Create default texture for untextured models
    byte_t lightGreyPixel[] = { 190, 190, 190, 255 };
    device->_defaultTexture = RuntimeTexture::CreateFromData(device.Get(), 1, 1, lightGreyPixel);

    // Create quad to use for 2D rendering
    RuntimeGeometry::Vertex quadVertices[] =
    {
        { Vector3(-1.0f, 1.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 0.0f) },
        { Vector3(1.0f, 1.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 0.0f) },
        { Vector3(1.0f, -1.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 1.0f) },
        { Vector3(-1.0f, -1.0f, 0.1f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 1.0f) },
    };

    uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

    device->_quad = RuntimeGeometry::CreateFromData(device.Get(), 1, _countof(quadVertices), quadVertices, _countof(indices), indices);

    return device.Detach();
}

// IGraphicsDevice
void GDKAPI DxGraphicsDevice::Clear(_In_ const Vector4& color)
{
    _context->ClearRenderTargetView(_backBuffer.Get(), &color.x);
}

void GDKAPI DxGraphicsDevice::ClearDepth(_In_ float depth)
{
    _context->ClearDepthStencilView(_depthStencil.Get(), D3D10_CLEAR_DEPTH, depth, 0);
}

void GDKAPI DxGraphicsDevice::Present()
{
    _swapChain->Present(_parameters.vsyncEnabled ? 1 : 0, 0);
}

void GDKAPI DxGraphicsDevice::SetFillMode(_In_ GraphicsFillMode fillMode)
{
    switch (fillMode)
    {
    case GraphicsFillMode::Solid:
        _context->RSSetState(nullptr);
        break;

    case GraphicsFillMode::Wireframe:
        _context->RSSetState(_wireframeRasterizerState.Get());
        break;
    }
}

HRESULT GDKAPI DxGraphicsDevice::CreateRuntimeGeometry(_In_ GDK::Content::IGeometryResource* data, _COM_Outptr_ IRuntimeGeometry** geometry)
{
    MODULE_GUARD_BEGIN

    CHECKHR(RuntimeGeometry::Create(this, data).CopyTo(geometry));

    MODULE_GUARD_END
}

HRESULT GDKAPI DxGraphicsDevice::CreateRuntimeTexture(_In_ GDK::Content::ITextureResource* data, _COM_Outptr_ IRuntimeTexture** texture)
{
    MODULE_GUARD_BEGIN

    CHECKHR(RuntimeTexture::Create(this, data).CopyTo(texture));

    MODULE_GUARD_END
}

HRESULT GDKAPI DxGraphicsDevice::ClearBoundResources()
{
    if (_boundGeometry)
    {
        _boundGeometry->Unbind(this);
    }

    BindTexture(0, _defaultTexture.Get());
    BindTexture(1, _defaultTexture.Get());

    return S_OK;
}

HRESULT GDKAPI DxGraphicsDevice::BindTexture(_In_ size_t slot, _In_opt_ IRuntimeTexture* texture)
{
    if (slot > _countof(_boundTexture))
    {
        return E_INVALIDARG;
    }

    if (_boundTexture[slot])
    {
        _boundTexture[slot]->Unbind(this);
    }

    _boundTexture[slot] = texture ? reinterpret_cast<RuntimeTexture*>(texture) : _defaultTexture;

    if (_boundTexture[slot])
    {
        _boundTexture[slot]->Bind(this);
    }

    return S_OK;
}

HRESULT GDKAPI DxGraphicsDevice::BindGeometry(_In_opt_ IRuntimeGeometry* geometry)
{
    if (_boundGeometry)
    {
        _boundGeometry->Unbind(this);
    }

    _boundGeometry = reinterpret_cast<RuntimeGeometry*>(geometry);

    if (_boundGeometry)
    {
        _boundGeometry->Bind(this);
    }

    return S_OK;
}

HRESULT GDKAPI DxGraphicsDevice::SetViewProjection(_In_ const Matrix& view, _In_ const Matrix& projection)
{
    _vsConstants.ViewProj = view * projection;

    UpdateVSConstantBuffer();

    return S_OK;
}

HRESULT GDKAPI DxGraphicsDevice::Draw(_In_ const Matrix& world)
{
    _vsConstants.World = world;

    Matrix::Inverse(world, &_vsConstants.InvTransWorld);
    _vsConstants.InvTransWorld.Transpose();

    UpdateVSConstantBuffer();

    if (_boundGeometry)
    {
        _boundGeometry->Draw(this);
    }

    return S_OK;
}

HRESULT GDKAPI DxGraphicsDevice::Draw2D(_In_ const RECT& destRect)
{
    Vector2 size(static_cast<float>(destRect.right - destRect.left) / 100.0f, static_cast<float>(destRect.bottom - destRect.top) / 100.0f);
    Vector2 position(destRect.left / 100.0f + size.x * 0.5f, destRect.top / 100.0f + size.y * 0.5f);

    Matrix world = Matrix::CreateTranslation(Vector3(position.x - 0.5f, position.y - 0.5f, 0.0f)) * Matrix::CreateScale(Vector3(size.x, size.y, 1.0f));

    auto previousGeometry = _boundGeometry;
    auto previousViewProj = _vsConstants.ViewProj;

    SetViewProjection(Matrix::Identity(), Matrix::Identity());

    BindGeometry(_quad.Get());
    Draw(world);

    BindGeometry(previousGeometry.Get());

    _vsConstants.ViewProj = previousViewProj;
    UpdateVSConstantBuffer();

    return S_OK;
}

void GDKAPI DxGraphicsDevice::UpdateVSConstantBuffer()
{
    _context->UpdateSubresource(_vsConstantBuffer.Get(), 0, nullptr, reinterpret_cast<const void*>(&_vsConstants), sizeof(_vsConstants), 0);
    _context->VSSetConstantBuffers(0, 1, _vsConstantBuffer.GetAddressOf());
}

void GDKAPI DxGraphicsDevice::UpdatePSConstantBuffer()
{
    _context->UpdateSubresource(_psConstantBuffer.Get(), 0, nullptr, reinterpret_cast<const void*>(&_psConstants), sizeof(_psConstants), 0);
    _context->PSSetConstantBuffers(0, 1, _psConstantBuffer.GetAddressOf());
}

} // Graphics
} // GDK
