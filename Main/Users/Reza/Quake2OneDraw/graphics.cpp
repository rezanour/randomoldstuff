#include "precomp.h"

static std::unique_ptr<uint8_t[]> ReadFile(_In_z_ const wchar_t* filename, _Out_ size_t* length)
{
    FileHandle file(CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    CHECKGLE(file.IsValid());

    DWORD len = GetFileSize(file.Get(), nullptr);
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[len]);

    DWORD read;
    CHECKGLE(ReadFile(file.Get(), buffer.get(), len, &read, nullptr));

    *length = static_cast<size_t>(len);

    return std::move(buffer);
}

_Use_decl_annotations_
Renderer::Renderer(HWND hwnd) :
    _hwnd(hwnd), _numIndices(0)
{
    RECT rc;
    GetClientRect(hwnd, &rc);

    uint32_t width = rc.right - rc.left;
    uint32_t height = rc.bottom - rc.top;

    ComPtr<IDXGIFactory1> factory;
    CHECKHR(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 2;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
    scd.BufferDesc.Width = width;
    scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Windowed = TRUE;

    UINT flags = 0;
    D3D_FEATURE_LEVEL requestedLevel = D3D_FEATURE_LEVEL_11_0;

#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    ComPtr<IDXGIAdapter1> adapter;

#if defined(USE_WARP)

    CHECKHR(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
            flags, &requestedLevel, 1, D3D11_SDK_VERSION, &scd, &_swapChain, &_device,
            nullptr, &_context));

    ComPtr<IDXGIDevice> dxgiDevice;
    ComPtr<IDXGIAdapter> dxgiAdapter;
    CHECKHR(_device.As(&dxgiDevice));
    CHECKHR(dxgiDevice->GetAdapter(&dxgiAdapter));
    CHECKHR(dxgiAdapter.As(&adapter));

#else

    UINT i = 0;
    ComPtr<IDXGIAdapter1> dxgiAdapter;
    while (SUCCEEDED(factory->EnumAdapters1(i++, &dxgiAdapter)))
    {
        DXGI_ADAPTER_DESC1 desc = {};
        CHECKHR(dxgiAdapter->GetDesc1(&desc))

        if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0)
        {
            if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(dxgiAdapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr,
                            flags, &requestedLevel, 1, D3D11_SDK_VERSION, &scd, &_swapChain, &_device,
                            nullptr, &_context)))
            {
                adapter = dxgiAdapter;
                break;
            }
        }
    }

#endif

    CHECKNOTNULL(adapter);

    DXGI_ADAPTER_DESC1 desc = {};
    CHECKHR(adapter->GetDesc1(&desc))
    DebugOut("Using adapter: %S (%u MB)\n", desc.Description, (UINT)(desc.DedicatedVideoMemory / (1024 * 1024)));

    ComPtr<ID3D11Texture2D> resource;
    CHECKHR(_swapChain->GetBuffer(0, IID_PPV_ARGS(&resource)));

    CHECKHR(_device->CreateRenderTargetView(resource.Get(), nullptr, &_backBuffer));

    D3D11_TEXTURE2D_DESC td = {};
    resource->GetDesc(&td);

    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.Format = DXGI_FORMAT_D32_FLOAT;
    td.Usage = D3D11_USAGE_DEFAULT;

    CHECKHR(_device->CreateTexture2D(&td, nullptr, &resource));
    CHECKHR(_device->CreateDepthStencilView(resource.Get(), nullptr, &_depthBuffer));

    _context->OMSetRenderTargets(1, _backBuffer.GetAddressOf(), _depthBuffer.Get());

    D3D11_VIEWPORT vp = {};
    vp.Width = static_cast<float>(width);
    vp.Height = static_cast<float>(height);
    vp.MaxDepth = 1.0f;

    _context->RSSetViewports(1, &vp);

    // Pipeline
    size_t length;
    auto buffer = ReadFile(L"vertexShader.cso", &length);
    CHECKHR(_device->CreateVertexShader(buffer.get(), length, nullptr, &_vertexShader));

    uint32_t numElems;
    auto elems = Bsp::GetInputElements(&numElems);
    CHECKHR(_device->CreateInputLayout(elems.get(), numElems, buffer.get(), length, &_inputLayout));

    buffer = ReadFile(L"pixelShader.cso", &length);
    CHECKHR(_device->CreatePixelShader(buffer.get(), length, nullptr, &_pixelShader));

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(VSPerFrame);
    bd.StructureByteStride = sizeof(VSPerFrame);
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.Usage = D3D11_USAGE_DYNAMIC;

    CHECKHR(_device->CreateBuffer(&bd, nullptr, &_vsPerFrame));

    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.ByteWidth = sizeof(uint32_t) * MaxIndices;
    bd.StructureByteStride = sizeof(uint32_t);
    bd.CPUAccessFlags = 0;
    bd.Usage = D3D11_USAGE_DEFAULT;

    CHECKHR(_device->CreateBuffer(&bd, nullptr, &_indexBuffer));

    D3D11_SAMPLER_DESC sd = {};
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    CHECKHR(_device->CreateSamplerState(&sd, &_sampler));

    _indices.reset(new uint32_t[MaxIndices]);
}

void Renderer::Clear()
{
    static const float clearColor[] = { 0, 0, 0, 1 };
    _context->ClearRenderTargetView(_backBuffer.Get(), clearColor);
    _context->ClearDepthStencilView(_depthBuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::Present()
{
    _swapChain->Present(0, 0);
    _context->OMSetRenderTargets(1, _backBuffer.GetAddressOf(), _depthBuffer.Get());
}

void Renderer::DrawBsp(const XMFLOAT4X4& cameraWorld, const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const std::unique_ptr<Bsp>& bsp)
{
    bsp->QueryVisibleTriangles(cameraWorld, projection, _indices.get(), MaxIndices, &_numIndices);

    uint32_t stride = Bsp::GetStride();
    uint32_t offset = 0;
    _context->IASetVertexBuffers(0, 1, bsp->GetVertexBuffer().GetAddressOf(), &stride, &offset);
    _context->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _context->IASetInputLayout(_inputLayout.Get());

    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->VSSetConstantBuffers(0, 1, _vsPerFrame.GetAddressOf());

    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);
    _context->PSSetShaderResources(0, 1, bsp->GetTextures().GetAddressOf());
    _context->PSSetSamplers(0, 1, _sampler.GetAddressOf());

    D3D11_MAPPED_SUBRESOURCE mapped;
    CHECKHR(_context->Map(_vsPerFrame.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));

    VSPerFrame* cb = reinterpret_cast<VSPerFrame*>(mapped.pData);
    cb->View = view;
    cb->Projection = projection;

    _context->Unmap(_vsPerFrame.Get(), 0);

    D3D11_BOX box = {};
    box.back = 1;
    box.bottom = 1;
    box.right = sizeof(uint32_t) * _numIndices;
    _context->UpdateSubresource(_indexBuffer.Get(), 0, &box, _indices.get(), box.right, 0);

    _context->DrawIndexed(_numIndices, 0, 0);
}
