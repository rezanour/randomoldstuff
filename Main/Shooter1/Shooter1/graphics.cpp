#include "precomp.h"
#include "graphics.h"
#include "draw2d.h"
#include "materialpriv.h"
#include "lambert.h"

//#define USE_WARP

static Graphics* g_graphics;

_Use_decl_annotations_
Graphics::Graphics(void* window) :
    _window(window)
{
    FindAdapterAndCreateDevice();
    CreateStateObjects();
    _draw2D.reset(new ::Draw2D(_context.Get()));
}

_Use_decl_annotations_
std::shared_ptr<TexturePool> Graphics::GetPoolWithSpace(uint32_t width, uint32_t height, DXGI_FORMAT format, bool supportsMips, uint32_t numTextures)
{
    // see if we have a match
    for (auto& existingPool : _texturePools)
    {
        if (existingPool->HasAvailableSpace(width, height, format, supportsMips, numTextures))
        {
            return existingPool;
        }
    }

    // need to create a new pool
    std::shared_ptr<TexturePool> pool(new TexturePool(_device.Get(), width, height, format, supportsMips, 8));
    _texturePools.push_back(pool);
    return pool;
}

_Use_decl_annotations_
std::shared_ptr<GeoPool> Graphics::GetPoolWithSpace(VertexFormat format, uint32_t numVertices, uint32_t numIndices)
{
    // see if we have a match
    for (auto& existingPool : _geoPools)
    {
        if (existingPool->HasAvailableSpace(format, numVertices, numIndices))
        {
            return existingPool;
        }
    }

    // need to create a new pool
    std::shared_ptr<GeoPool> pool(new GeoPool(_device.Get(), format, 150000, 150000));
    _geoPools.push_back(pool);
    return pool;
}

_Use_decl_annotations_
void Graphics::Draw2D(const Texture& texture, const RECT& source, const RECT& dest, const XMFLOAT4& color)
{
    auto lock = LockContext();

    _draw2D->Add(texture, source, dest, color);
}

_Use_decl_annotations_
void Graphics::Draw2DFullScreen(const Texture& texture, const RECT& source, const RECT& dest, const XMFLOAT4& color)
{
    auto lock = LockContext();

    _draw2D->AddFullScreen(texture, source, dest, color);
}

_Use_decl_annotations_
void Graphics::SetMaterial(MaterialType material)
{
    if (_activeMaterial == material)
    {
        // no-op
        return;
    }

    _activeMaterial = material;
    uint32_t index = (uint32_t)_activeMaterial;

    if (_materials[index] == nullptr)
    {
        LoadMaterial(material);
    }

    _materials[index]->ApplyMaterial();
}

_Use_decl_annotations_
void Graphics::Draw3D(const XMFLOAT4X4& view, const XMFLOAT4X4& projection, GameObject** objects, uint32_t numObjects)
{
    auto lock = LockContext();

    uint32_t index = (uint32_t)_activeMaterial;
    _materials[index]->Draw(view, projection, objects, numObjects);
}

void Graphics::Present()
{
    _draw2D->Draw();
    // Drawing 2D clears active material, so reflect that
    _activeMaterial = MaterialType::Invalid;

    auto lock = GetGraphics().LockContext();

    _swapChain->Present(0, 0);
    _context->OMSetRenderTargets(1, _renderTarget.GetAddressOf(), _depthStencil.Get());

    static const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    _context->ClearRenderTargetView(_renderTarget.Get(), clearColor);
    _context->ClearDepthStencilView(_depthStencil.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphics::FindAdapterAndCreateDevice()
{
    ComPtr<IDXGIFactory1> factory;

    CHECKHR(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

    auto& config = GetConfig();

    DXGI_SWAP_CHAIN_DESC swd = {};
    swd.BufferCount = 3;
    swd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swd.BufferDesc.Height = config.ScreenHeight;
    swd.BufferDesc.RefreshRate.Denominator = 1;
    swd.BufferDesc.RefreshRate.Numerator = 60;
    swd.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
    swd.BufferDesc.Width = config.ScreenWidth;
    swd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swd.OutputWindow = (HWND)_window;
    swd.SampleDesc.Count = 1;
    swd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swd.Windowed = TRUE;

    D3D_FEATURE_LEVEL requestedLevel = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL featureLevel;

    UINT flags = 0;

#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    ComPtr<IDXGIAdapter1> adapter;

#if defined(USE_WARP)

    CHECKHR(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
            flags, &requestedLevel, 1, D3D11_SDK_VERSION, &swd, &_swapChain, &_device,
            &featureLevel, &_context));

    ComPtr<IDXGIDevice> dxgiDevice;
    CHECKHR(_device.As(&dxgiDevice));
    CHECKHR(dxgiDevice->GetAdapter(&_adapter));
    CHECKHR(_adapter.As(&adapter));

    DXGI_ADAPTER_DESC1 desc = {};
    CHECKHR(adapter->GetDesc1(&desc))

    DebugOut("Using adapter: %S (%u MB)\n", desc.Description, (UINT)(desc.DedicatedVideoMemory / (1024 * 1024)));

#else

    UINT i = 0;
    while (SUCCEEDED(factory->EnumAdapters1(i++, &adapter)))
    {
        DXGI_ADAPTER_DESC1 desc = {};
        CHECKHR(adapter->GetDesc1(&desc))

        if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0)
        {
            if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr,
                            flags, &requestedLevel, 1, D3D11_SDK_VERSION, &swd, &_swapChain, &_device,
                            &featureLevel, &_context)))
            {
                _adapter = adapter;
                DebugOut("Using adapter: %S (%u MB)\n", desc.Description, (UINT)(desc.DedicatedVideoMemory / (1024 * 1024)));
                break;
            }
        }
    }

#endif

    CHECKNOTNULL(_adapter);

    ComPtr<ID3D11Texture2D> resource;
    CHECKHR(_swapChain->GetBuffer(0, IID_PPV_ARGS(&resource)));

    CHECKHR(_device->CreateRenderTargetView(resource.Get(), nullptr, &_renderTarget));

    D3D11_TEXTURE2D_DESC td = {};
    resource->GetDesc(&td);

    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.Format = DXGI_FORMAT_D32_FLOAT;
    td.Usage = D3D11_USAGE_DEFAULT;

    CHECKHR(_device->CreateTexture2D(&td, nullptr, &resource));
    CHECKHR(_device->CreateDepthStencilView(resource.Get(), nullptr, &_depthStencil));

    _context->OMSetRenderTargets(1, _renderTarget.GetAddressOf(), _depthStencil.Get());

    D3D11_VIEWPORT vp = {};
    vp.Width = static_cast<float>(GetConfig().ScreenWidth);
    vp.Height = static_cast<float>(GetConfig().ScreenHeight);
    vp.MaxDepth = 1.0f;

    _context->RSSetViewports(1, &vp);
}

_Use_decl_annotations_
void Graphics::LoadMaterial(MaterialType material)
{
    //
    // TODO: Load material impl based on available options & hardware capabilities
    //
    switch (material)
    {
    case MaterialType::Matte:
        _materials[(uint32_t)MaterialType::Matte].reset(new Lambert());
        break;

    default:
        assert(false);
        break;
    }
}

void Graphics::CreateStateObjects()
{
    D3D11_SAMPLER_DESC sd = {};
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.MaxLOD = D3D11_FLOAT32_MAX;

    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    CHECKHR(_device->CreateSamplerState(&sd, &_pointWrapSampler));

    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    CHECKHR(_device->CreateSamplerState(&sd, &_linearWrapSampler));
}

_Use_decl_annotations_
void GraphicsStartup(void* window)
{
    assert(!g_graphics);

    delete g_graphics;

    g_graphics = new Graphics(window);
    DebugOut("Graphics initialized.\n");
}

void GraphicsShutdown()
{
    assert(g_graphics);

    delete g_graphics;
    g_graphics = nullptr;
    DebugOut("Graphics shutdown.\n");
}

Graphics& GetGraphics()
{
    assert(g_graphics);
    return *g_graphics;
}

//
// TODO: Move this. Doesn't belong in graphics.cpp
//
std::unique_ptr<uint8_t[]> ReadFile(_In_z_ const char* filename, _Out_ size_t* length)
{
    FileHandle file(CreateFile(filename, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    CHECKGLE(file.IsValid());

    DWORD len = GetFileSize(file.Get(), nullptr);
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[len]);

    DWORD read;
    CHECKGLE(ReadFile(file.Get(), buffer.get(), len, &read, nullptr));

    *length = static_cast<size_t>(len);

    return std::move(buffer);
}
