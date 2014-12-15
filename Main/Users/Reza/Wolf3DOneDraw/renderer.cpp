#include "precomp.h"
#include "renderer.h"

_Use_decl_annotations_
HRESULT Renderer::Create(HWND hwnd, const wchar_t* contentRoot, Renderer** renderer)
{
    assert(hwnd);
    assert(renderer);

    *renderer = nullptr;

    std::unique_ptr<Renderer> r(new (std::nothrow) Renderer);
    if (!r)
    {
        return E_OUTOFMEMORY;
    }

    HRESULT hr = r->Initialize(hwnd, contentRoot);
    if (SUCCEEDED(hr))
    {
        *renderer = r.release();
    }

    return hr;
}

Renderer::Renderer() :
    _numInstances(0)
{
}

HRESULT Renderer::ChangeLevel(uint32_t level)
{
    assert(level < _levels.size());

    _numInstances = 0;
    _instanceBuffer3D.Reset();

    auto& l = _levels[level];

    static const uint32_t MaxWalls = 98;

    std::vector<Draw3DInstance> instances;
    for (uint32_t i = 0; i < l.Data.size(); ++i)
    {
        uint32_t id = (l.Data[i] - 1) * 2 + 1;
        if (id > 0 && id < MaxWalls)
        {
            Draw3DInstance instance;
            instance.CellIndex = i;
            instance.TexIndex = id;
            instances.push_back(instance);
        }
    }

    _numInstances = (uint32_t)instances.size();

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Draw3DInstance) * _numInstances;
    bd.StructureByteStride = sizeof(Draw3DInstance);

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = instances.data();
    init.SysMemPitch = bd.ByteWidth;

    HRESULT hr = _device->CreateBuffer(&bd, &init, &_instanceBuffer3D);
    if (FAILED(hr))
    {
        return hr;
    }

    return hr;
}

void Renderer::Clear()
{
    static const uint32_t stride = sizeof(ClearVertex);
    static const uint32_t offset = 0;

    // disable depth buffer for clear pass
    _context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
    _context->IASetInputLayout(_inputLayoutClear.Get());
    _context->IASetVertexBuffers(0, 1, _vertexBufferClear.GetAddressOf(), &stride, &offset);
    _context->VSSetShader(_vertexShaderClear.Get(), nullptr, 0);
    _context->PSSetShader(_pixelShaderClear.Get(), nullptr, 0);
    _context->PSSetConstantBuffers(0, 1, _constantBufferClear.GetAddressOf());
    _context->Draw(6, 0);

    _context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
    _context->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::Draw3DView(const XMFLOAT4X4& view, const XMFLOAT4X4& projection)
{
    static const uint32_t strides[] = { sizeof(Draw3DVertex), sizeof(Draw3DInstance) };
    static const uint32_t offsets[] = { 0, 0 };

    if (_numInstances > 0)
    {
        ID3D11Buffer* buffers[] = { _vertexBuffer3D.Get(), _instanceBuffer3D.Get() };
        _context->IASetInputLayout(_inputLayout3D.Get());
        _context->IASetVertexBuffers(0, _countof(buffers), buffers, strides, offsets);
        _context->IASetIndexBuffer(_indexBuffer3D.Get(), DXGI_FORMAT_R32_UINT, 0);

        D3D11_MAPPED_SUBRESOURCE mapped;
        HRESULT hr = _context->Map(_constantBuffer3D.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        if (FAILED(hr))
        {
            assert(false);
            return;
        }

        Draw3DConstants* constants = (Draw3DConstants*)mapped.pData;
        constants->View = view;
        constants->Projection = projection;

        _context->Unmap(_constantBuffer3D.Get(), 0);

        _context->VSSetShader(_vertexShader3D.Get(), nullptr, 0);
        _context->VSSetConstantBuffers(0, 1, _constantBuffer3D.GetAddressOf());

        _context->PSSetShader(_pixelShader3D.Get(), nullptr, 0);
        _context->PSSetShaderResources(0, 1, _textures.GetAddressOf());
        _context->PSSetSamplers(0, 1, _sampler.GetAddressOf());

        _context->DrawIndexedInstanced(24, _numInstances, 0, 0, 0);
    }
}

void Renderer::Present()
{
    _swapChain->Present(0, 0);
}

_Use_decl_annotations_
HRESULT Renderer::Initialize(HWND hwnd, const wchar_t* contentRoot)
{
    _contentRoot.assign(contentRoot);

    RECT rc;
    GetClientRect(hwnd, &rc);
    uint32_t width = rc.right - rc.left;
    uint32_t height = rc.bottom - rc.top;

    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 2;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    scd.Windowed = TRUE;

    UINT flags = 0;

#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
                    &featureLevel, 1, D3D11_SDK_VERSION, &scd, &_swapChain, &_device, nullptr, &_context);
    if (FAILED(hr))
    {
        return hr;
    }

    ComPtr<ID3D11Texture2D> resource;
    hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(&resource));
    if (FAILED(hr))
    {
        return hr;
    }

    hr = _device->CreateRenderTargetView(resource.Get(), nullptr, &_renderTargetView);
    if (FAILED(hr))
    {
        return hr;
    }

    D3D11_TEXTURE2D_DESC td = {};
    resource->GetDesc(&td);
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.Format = DXGI_FORMAT_D32_FLOAT;

    hr = _device->CreateTexture2D(&td, nullptr, resource.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        return hr;
    }

    hr = _device->CreateDepthStencilView(resource.Get(), nullptr, &_depthStencilView);
    if (FAILED(hr))
    {
        return hr;
    }

    D3D11_VIEWPORT vp = {};
    vp.Width = (float)width;
    vp.Height = (float)height;
    vp.MaxDepth = 1.0f;
    _context->RSSetViewports(1, &vp);

    hr = InitializeClearScreen((float)width, (float)height);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = InitialzeDraw3D();
    if (FAILED(hr))
    {
        return hr;
    }

    hr = LoadWolf3DLevels();
    if (FAILED(hr))
    {
        assert(false);
        return hr;
    }

    hr = LoadWolf3DTextures();
    if (FAILED(hr))
    {
        assert(false);
        return hr;
    }

    _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return hr;
}

HRESULT Renderer::InitializeClearScreen(float width, float height)
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(ClearConstants);
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.StructureByteStride = bd.ByteWidth;
    bd.Usage = D3D11_USAGE_DYNAMIC;

    ClearConstants constants;
    constants.CeilingColor = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    constants.FloorColor = XMFLOAT4(0.4f, 0.4f, 0.45f, 1.0f);
    constants.ScreenSize = XMFLOAT2(width, height);

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = &constants;
    init.SysMemPitch = sizeof(constants);

    HRESULT hr = _device->CreateBuffer(&bd, &init, &_constantBufferClear);
    if (FAILED(hr))
    {
        return hr;
    }

    std::unique_ptr<uint8_t[]> buffer;
    size_t size;
    hr = LoadFile(L"ClearScreenVS.cso", &buffer, &size);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = _device->CreateVertexShader(buffer.get(), size, nullptr, &_vertexShaderClear);
    if (FAILED(hr))
    {
        return hr;
    }

    D3D11_INPUT_ELEMENT_DESC elems[1] = {};
    elems[0].Format = DXGI_FORMAT_R32G32_FLOAT;
    elems[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elems[0].SemanticName = "POSITION";

    hr = _device->CreateInputLayout(elems, _countof(elems), buffer.get(), size, &_inputLayoutClear);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = LoadFile(L"ClearScreenPS.cso", &buffer, &size);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = _device->CreatePixelShader(buffer.get(), size, nullptr, &_pixelShaderClear);
    if (FAILED(hr))
    {
        return hr;
    }

    ClearVertex vertices[] = 
    {
        { XMFLOAT2(-1, 1) },
        { XMFLOAT2(1, 1) },
        { XMFLOAT2(1, -1) },
        { XMFLOAT2(-1, 1) },
        { XMFLOAT2(1, -1) },
        { XMFLOAT2(-1, -1) },
    };

    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(vertices);
    bd.StructureByteStride = sizeof(vertices[0]);
    bd.CPUAccessFlags = 0;
    bd.Usage = D3D11_USAGE_DEFAULT;

    init.pSysMem = vertices;
    init.SysMemPitch = bd.ByteWidth;

    hr = _device->CreateBuffer(&bd, &init, &_vertexBufferClear);
    if (FAILED(hr))
    {
        return hr;
    }

    return hr;
}

HRESULT Renderer::InitialzeDraw3D()
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(Draw3DConstants);
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.StructureByteStride = bd.ByteWidth;
    bd.Usage = D3D11_USAGE_DYNAMIC;

    HRESULT hr = _device->CreateBuffer(&bd, nullptr, &_constantBuffer3D);
    if (FAILED(hr))
    {
        return hr;
    }

    std::unique_ptr<uint8_t[]> buffer;
    size_t size;
    hr = LoadFile(L"Draw3DVS.cso", &buffer, &size);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = _device->CreateVertexShader(buffer.get(), size, nullptr, &_vertexShader3D);
    if (FAILED(hr))
    {
        return hr;
    }

    D3D11_INPUT_ELEMENT_DESC elems[5] = {};
    elems[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    elems[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elems[0].SemanticName = "POSITION";

    elems[1].AlignedByteOffset = sizeof(XMFLOAT3);
    elems[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    elems[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elems[1].SemanticName = "NORMAL";

    elems[2].AlignedByteOffset = 2 * sizeof(XMFLOAT3);
    elems[2].Format = DXGI_FORMAT_R32G32_FLOAT;
    elems[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elems[2].SemanticName = "TEXCOORD";

    elems[3].Format = DXGI_FORMAT_R32_UINT;
    elems[3].InputSlot = 1;
    elems[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    elems[3].InstanceDataStepRate = 1;
    elems[3].SemanticName = "TEXCOORD";
    elems[3].SemanticIndex = 1;

    elems[4].AlignedByteOffset = sizeof(uint32_t);
    elems[4].Format = DXGI_FORMAT_R32_UINT;
    elems[4].InputSlot = 1;
    elems[4].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    elems[4].InstanceDataStepRate = 1;
    elems[4].SemanticName = "TEXCOORD";
    elems[4].SemanticIndex = 2;

    hr = _device->CreateInputLayout(elems, _countof(elems), buffer.get(), size, &_inputLayout3D);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = LoadFile(L"Draw3DPS.cso", &buffer, &size);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = _device->CreatePixelShader(buffer.get(), size, nullptr, &_pixelShader3D);
    if (FAILED(hr))
    {
        return hr;
    }

    // 4 side faces of cube
    Draw3DVertex vertices[] = 
    {
        // front
        { XMFLOAT3(-0.5, 0.5, -0.5), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
        { XMFLOAT3(0.5, 0.5, -0.5), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 0) },
        { XMFLOAT3(0.5, -0.5, -0.5), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 1) },
        { XMFLOAT3(-0.5,- 0.5, -0.5), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 1) },
        // back
        { XMFLOAT3(0.5, 0.5, 0.5), XMFLOAT3(0, 0, 1), XMFLOAT2(0, 0) },
        { XMFLOAT3(-0.5, 0.5, 0.5), XMFLOAT3(0, 0, 1), XMFLOAT2(1, 0) },
        { XMFLOAT3(-0.5, -0.5, 0.5), XMFLOAT3(0, 0, 1), XMFLOAT2(1, 1) },
        { XMFLOAT3(0.5,- 0.5, 0.5), XMFLOAT3(0, 0, 1), XMFLOAT2(0, 1) },
        // right
        { XMFLOAT3(0.5, 0.5, -0.5), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 0) },
        { XMFLOAT3(0.5, 0.5, 0.5), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 0) },
        { XMFLOAT3(0.5, -0.5, 0.5), XMFLOAT3(1, 0, 0), XMFLOAT2(1, 1) },
        { XMFLOAT3(0.5,- 0.5, -0.5), XMFLOAT3(1, 0, 0), XMFLOAT2(0, 1) },
        // left
        { XMFLOAT3(-0.5, 0.5, 0.5), XMFLOAT3(-1, 0, 0), XMFLOAT2(0, 0) },
        { XMFLOAT3(-0.5, 0.5, -0.5), XMFLOAT3(-1, 0, 0), XMFLOAT2(1, 0) },
        { XMFLOAT3(-0.5, -0.5, -0.5), XMFLOAT3(-1, 0, 0), XMFLOAT2(1, 1) },
        { XMFLOAT3(-0.5,- 0.5, 0.5), XMFLOAT3(-1, 0, 0), XMFLOAT2(0, 1) },
    };

    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(vertices);
    bd.StructureByteStride = sizeof(vertices[0]);
    bd.CPUAccessFlags = 0;
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = vertices;
    init.SysMemPitch = bd.ByteWidth;

    hr = _device->CreateBuffer(&bd, &init, &_vertexBuffer3D);
    if (FAILED(hr))
    {
        return hr;
    }

    uint32_t indices[] =
    {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15
    };

    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.ByteWidth = sizeof(indices);
    bd.StructureByteStride = sizeof(indices[0]);

    init.pSysMem = indices;
    init.SysMemPitch = bd.ByteWidth;

    hr = _device->CreateBuffer(&bd, &init, &_indexBuffer3D);
    if (FAILED(hr))
    {
        return hr;
    }

    D3D11_SAMPLER_DESC sd = {};
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

    hr = _device->CreateSamplerState(&sd, &_sampler);

    return hr;
}

_Use_decl_annotations_
HRESULT Renderer::LoadFile(const wchar_t* filename, std::unique_ptr<uint8_t[]>* buffer, size_t* size)
{
    assert(buffer);
    assert(size);

    *size = 0;
    buffer->reset();

    FileHandle file(CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file.IsValid())
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    DWORD fileSize = GetFileSize(file.Get(), nullptr);
    std::unique_ptr<uint8_t[]> data(new (std::nothrow) uint8_t[fileSize]);
    if (!data)
    {
        return E_OUTOFMEMORY;
    }

    DWORD read;
    if (!ReadFile(file.Get(), data.get(), fileSize, &read, nullptr))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    *size = (size_t)fileSize;
    buffer->swap(data);

    return S_OK;
}

//
// WL6 Data
//
#pragma pack(push,1)

struct LevelData
{
    uint32_t Layer1Offset;
    uint32_t Layer2Offset;
    uint32_t Layer3Offset;
    uint16_t Layer1SizeBytes;
    uint16_t Layer2SizeBytes;
    uint16_t Layer3SizeBytes;
    uint16_t LayerWidth;
    uint16_t LayerHeight;
    char Name[16];
    char Signature[4];
};

struct MapFileHeader
{
    static const uint16_t MagicNumber = 0xABCD;
    uint16_t Marker;
};

struct VSwapHeader
{
    uint16_t NumChunks;
    uint16_t SpritesOffset;
    uint16_t SoundsOffset;
};

// 256 palette for Wolf3D graphics
struct PalEntry
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
};

static const PalEntry Wolf3DPalette[256] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0xA8, 0x00, 0xA8, 0x00, 0x00, 0xA8, 0xA8, 0xA8, 0x00, 0x00, 0xA8,
    0x00, 0xA8, 0xA8, 0x54, 0x00, 0xA8, 0xA8, 0xA8, 0x54, 0x54, 0x54, 0x54, 0x54, 0xFC, 0x54, 0xFC,
    0x54, 0x54, 0xFC, 0xFC, 0xFC, 0x54, 0x54, 0xFC, 0x54, 0xFC, 0xFC, 0xFC, 0x54, 0xFC, 0xFC, 0xFC,
    0xEC, 0xEC, 0xEC, 0xDC, 0xDC, 0xDC, 0xD0, 0xD0, 0xD0, 0xC0, 0xC0, 0xC0, 0xB4, 0xB4, 0xB4, 0xA8,
    0xA8, 0xA8, 0x98, 0x98, 0x98, 0x8C, 0x8C, 0x8C, 0x7C, 0x7C, 0x7C, 0x70, 0x70, 0x70, 0x64, 0x64,
    0x64, 0x54, 0x54, 0x54, 0x48, 0x48, 0x48, 0x38, 0x38, 0x38, 0x2C, 0x2C, 0x2C, 0x20, 0x20, 0x20,
    0xFC, 0x00, 0x00, 0xEC, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xD4, 0x00, 0x00, 0xC8, 0x00, 0x00, 0xBC,
    0x00, 0x00, 0xB0, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x98, 0x00, 0x00, 0x88, 0x00, 0x00, 0x7C, 0x00,
    0x00, 0x70, 0x00, 0x00, 0x64, 0x00, 0x00, 0x58, 0x00, 0x00, 0x4C, 0x00, 0x00, 0x40, 0x00, 0x00,
    0xFC, 0xD8, 0xD8, 0xFC, 0xB8, 0xB8, 0xFC, 0x9C, 0x9C, 0xFC, 0x7C, 0x7C, 0xFC, 0x5C, 0x5C, 0xFC,
    0x40, 0x40, 0xFC, 0x20, 0x20, 0xFC, 0x00, 0x00, 0xFC, 0xA8, 0x5C, 0xFC, 0x98, 0x40, 0xFC, 0x88,
    0x20, 0xFC, 0x78, 0x00, 0xE4, 0x6C, 0x00, 0xCC, 0x60, 0x00, 0xB4, 0x54, 0x00, 0x9C, 0x4C, 0x00,
    0xFC, 0xFC, 0xD8, 0xFC, 0xFC, 0xB8, 0xFC, 0xFC, 0x9C, 0xFC, 0xFC, 0x7C, 0xFC, 0xF8, 0x5C, 0xFC,
    0xF4, 0x40, 0xFC, 0xF4, 0x20, 0xFC, 0xF4, 0x00, 0xE4, 0xD8, 0x00, 0xCC, 0xC4, 0x00, 0xB4, 0xAC,
    0x00, 0x9C, 0x9C, 0x00, 0x84, 0x84, 0x00, 0x70, 0x6C, 0x00, 0x58, 0x54, 0x00, 0x40, 0x40, 0x00,
    0xD0, 0xFC, 0x5C, 0xC4, 0xFC, 0x40, 0xB4, 0xFC, 0x20, 0xA0, 0xFC, 0x00, 0x90, 0xE4, 0x00, 0x80,
    0xCC, 0x00, 0x74, 0xB4, 0x00, 0x60, 0x9C, 0x00, 0xD8, 0xFC, 0xD8, 0xBC, 0xFC, 0xB8, 0x9C, 0xFC,
    0x9C, 0x80, 0xFC, 0x7C, 0x60, 0xFC, 0x5C, 0x40, 0xFC, 0x40, 0x20, 0xFC, 0x20, 0x00, 0xFC, 0x00,
    0x00, 0xFC, 0x00, 0x00, 0xEC, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xD4, 0x00, 0x04, 0xC8, 0x00, 0x04,
    0xBC, 0x00, 0x04, 0xB0, 0x00, 0x04, 0xA4, 0x00, 0x04, 0x98, 0x00, 0x04, 0x88, 0x00, 0x04, 0x7C,
    0x00, 0x04, 0x70, 0x00, 0x04, 0x64, 0x00, 0x04, 0x58, 0x00, 0x04, 0x4C, 0x00, 0x04, 0x40, 0x00,
    0xD8, 0xFC, 0xFC, 0xB8, 0xFC, 0xFC, 0x9C, 0xFC, 0xFC, 0x7C, 0xFC, 0xF8, 0x5C, 0xFC, 0xFC, 0x40,
    0xFC, 0xFC, 0x20, 0xFC, 0xFC, 0x00, 0xFC, 0xFC, 0x00, 0xE4, 0xE4, 0x00, 0xCC, 0xCC, 0x00, 0xB4,
    0xB4, 0x00, 0x9C, 0x9C, 0x00, 0x84, 0x84, 0x00, 0x70, 0x70, 0x00, 0x58, 0x58, 0x00, 0x40, 0x40,
    0x5C, 0xBC, 0xFC, 0x40, 0xB0, 0xFC, 0x20, 0xA8, 0xFC, 0x00, 0x9C, 0xFC, 0x00, 0x8C, 0xE4, 0x00,
    0x7C, 0xCC, 0x00, 0x6C, 0xB4, 0x00, 0x5C, 0x9C, 0xD8, 0xD8, 0xFC, 0xB8, 0xBC, 0xFC, 0x9C, 0x9C,
    0xFC, 0x7C, 0x80, 0xFC, 0x5C, 0x60, 0xFC, 0x40, 0x40, 0xFC, 0x20, 0x24, 0xFC, 0x00, 0x04, 0xFC,
    0x00, 0x00, 0xFC, 0x00, 0x00, 0xEC, 0x00, 0x00, 0xE0, 0x00, 0x00, 0xD4, 0x00, 0x00, 0xC8, 0x00,
    0x00, 0xBC, 0x00, 0x00, 0xB0, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x98, 0x00, 0x00, 0x88, 0x00, 0x00,
    0x7C, 0x00, 0x00, 0x70, 0x00, 0x00, 0x64, 0x00, 0x00, 0x58, 0x00, 0x00, 0x4C, 0x00, 0x00, 0x40,
    0x28, 0x28, 0x28, 0xFC, 0xE0, 0x34, 0xFC, 0xD4, 0x24, 0xFC, 0xCC, 0x18, 0xFC, 0xC0, 0x08, 0xFC,
    0xB4, 0x00, 0xB4, 0x20, 0xFC, 0xA8, 0x00, 0xFC, 0x98, 0x00, 0xE4, 0x80, 0x00, 0xCC, 0x74, 0x00,
    0xB4, 0x60, 0x00, 0x9C, 0x50, 0x00, 0x84, 0x44, 0x00, 0x70, 0x34, 0x00, 0x58, 0x28, 0x00, 0x40,
    0xFC, 0xD8, 0xFC, 0xFC, 0xB8, 0xFC, 0xFC, 0x9C, 0xFC, 0xFC, 0x7C, 0xFC, 0xFC, 0x5C, 0xFC, 0xFC,
    0x40, 0xFC, 0xFC, 0x20, 0xFC, 0xFC, 0x00, 0xFC, 0xE0, 0x00, 0xE4, 0xC8, 0x00, 0xCC, 0xB4, 0x00,
    0xB4, 0x9C, 0x00, 0x9C, 0x84, 0x00, 0x84, 0x6C, 0x00, 0x70, 0x58, 0x00, 0x58, 0x40, 0x00, 0x40,
    0xFC, 0xE8, 0xDC, 0xFC, 0xE0, 0xD0, 0xFC, 0xD8, 0xC4, 0xFC, 0xD4, 0xBC, 0xFC, 0xCC, 0xB0, 0xFC,
    0xC4, 0xA4, 0xFC, 0xBC, 0x9C, 0xFC, 0xB8, 0x90, 0xFC, 0xB0, 0x80, 0xFC, 0xA4, 0x70, 0xFC, 0x9C,
    0x60, 0xF0, 0x94, 0x5C, 0xE8, 0x8C, 0x58, 0xDC, 0x88, 0x54, 0xD0, 0x80, 0x50, 0xC8, 0x7C, 0x4C,
    0xBC, 0x78, 0x48, 0xB4, 0x70, 0x44, 0xA8, 0x68, 0x40, 0xA0, 0x64, 0x3C, 0x9C, 0x60, 0x38, 0x90,
    0x5C, 0x34, 0x88, 0x58, 0x30, 0x80, 0x50, 0x2C, 0x74, 0x4C, 0x28, 0x6C, 0x48, 0x24, 0x5C, 0x40,
    0x20, 0x54, 0x3C, 0x1C, 0x48, 0x38, 0x18, 0x40, 0x30, 0x18, 0x38, 0x2C, 0x14, 0x28, 0x20, 0x0C,
    0x60, 0x00, 0x64, 0x00, 0x64, 0x64, 0x00, 0x60, 0x60, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x2C, 0x30,
    0x24, 0x10, 0x48, 0x00, 0x48, 0x50, 0x00, 0x50, 0x00, 0x00, 0x34, 0x1C, 0x1C, 0x1C, 0x4C, 0x4C,
    0x4C, 0x5C, 0x5C, 0x5C, 0x40, 0x40, 0x40, 0x30, 0x30, 0x30, 0x34, 0x34, 0x34, 0xD8, 0xF4, 0xF4,
    0xB8, 0xE8, 0xE8, 0x9C, 0xDC, 0xDC, 0x74, 0xC8, 0xC8, 0x48, 0xC0, 0xC0, 0x20, 0xB4, 0xB4, 0x20,
    0xB0, 0xB0, 0x00, 0xA4, 0xA4, 0x00, 0x98, 0x98, 0x00, 0x8C, 0x8C, 0x00, 0x84, 0x84, 0x00, 0x7C,
    0x7C, 0x00, 0x78, 0x78, 0x00, 0x74, 0x74, 0x00, 0x70, 0x70, 0x00, 0x6C, 0x6C, 0x98, 0x00, 0x88
};

#pragma pack(pop)

#if 0
struct w3d_spriteData_t
{
    uint16_t leftpix;
    uint16_t rightpix;
    uint16_t offsets[64];
};

struct w3d_layer_t
{
    uint32_t  offset;
    uint16_t  sizeBytes;
    uint16_t  width;
    uint16_t  height;
    uint16_t* data; // memory was allocated using malloc()
};

struct w3d_levelData_t
{
    uint16_t* layer1; // walls (memory was allocated using malloc())
    uint16_t* layer2; // objects (memory was allocated using malloc())
    uint16_t* layer3; // other (memory was allocated using malloc())
};

struct w3d_picture_t
{
    uint16_t width;
    uint16_t height;
};

struct w3d_huffnode_t
{
    uint16_t bit0;
    uint16_t bit1;
};

struct w3d_huffnode32_t
{
    uint32_t bit0;
    uint32_t bit1;
};

struct w3d_paletteentry_t
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

enum w3d_layerType_t
{
    wallsLayer,
    itemsLayer,
    miscLayer
};

#pragma pack(pop)

uint16_t MAX_WALL_SPRITES         = 98;
uint16_t CARMACK_EXPAND_NEARTAG   = 0xA7;
uint16_t CARMACK_EXPAND_FARTAG    = 0xA8;
uint16_t EXPANDED_MAP_SIZE        = 64 * 64;

#endif

void UncompressCarmack(const uint16_t* compressed, uint16_t* decompressed, uint16_t decompressedSize)
{
    static const uint8_t NearTag = 0xA7;
    static const uint8_t FarTag = 0xA8;

    uint16_t word;
    uint8_t high;
    uint16_t count, offset;

    uint16_t* base = decompressed;
    uint16_t* end = decompressed + decompressedSize / 2;
    uint16_t* copy = nullptr;

    while (decompressed < end)
    {
        word = *compressed++;
        high = word >> 8;
        count = word & 0xff;

        if (high == NearTag || high == FarTag)
        {
            if (count == 0)
            {
                // special case, not really near tag (just a word of that same value)
                const uint8_t* p = (const uint8_t*)compressed;
                word |= *p;
                ++p;
                compressed = (const uint16_t*)p;
                *decompressed++ = word;
            }
            else if (high == NearTag)
            {
                const uint8_t* p = (const uint8_t*)compressed;
                offset = *p;
                ++p;
                compressed = (const uint16_t*)p;

                copy = decompressed - offset;
                while (count--)
                {
                    *decompressed++ = *copy++;
                }
            }
            else
            {
                offset = *compressed++;
                copy = base + offset;
                while (count--)
                {
                    *decompressed++ = *copy++;
                }
            }
        }
        else
        {
            *decompressed++ = word;
        }
    }
}

void ExpandRLEW(const uint16_t* compressed, uint16_t* expanded, uint16_t expandedSize)
{
    uint16_t* end = nullptr;
    uint16_t value;
    uint16_t count;
    uint32_t i;

    end = expanded + expandedSize / 2;

    while (expanded < end)
    {
        value = *compressed++;
        if(value != MapFileHeader::MagicNumber)
        {
            // uncompressed
            *expanded++ = value;
        }
        else 
        {
            // compressed
            count = *compressed++;
            value = *compressed++;
            for(i = 0; i < count; ++i)
            {
                *expanded++ = value;
            }
        }
    };
}

HRESULT Renderer::LoadWolf3DLevels()
{
    size_t size;
    std::unique_ptr<uint8_t[]> buffer;
    wchar_t filename[1024];
    swprintf_s(filename, L"%s\\maphead.wl6", _contentRoot.c_str());
    HRESULT hr = LoadFile(filename, &buffer, &size);
    if (FAILED(hr))
    {
        return hr;
    }

    MapFileHeader* header = (MapFileHeader*)buffer.get();
    if (header->Marker != MapFileHeader::MagicNumber)
    {
        return E_INVALIDARG;
    }

    uint32_t* offsets = (uint32_t*)(buffer.get() + sizeof(MapFileHeader));

    std::unique_ptr<uint8_t[]> mapData;
    swprintf_s(filename, L"%s\\gamemaps.wl6", _contentRoot.c_str());
    hr = LoadFile(filename, &mapData, &size);
    if (FAILED(hr))
    {
        return hr;
    }

    while(*offsets != 0)
    {
        LevelData* levelData = (LevelData*)(mapData.get() + *offsets);

        Level level;
        level.Width = levelData->LayerWidth;
        level.Height = levelData->LayerHeight;
        strcpy_s(level.Name, levelData->Name);
        level.Data.resize(level.Width * level.Height);

        uint16_t* compressed = (uint16_t*)(mapData.get() + levelData->Layer1Offset);
        uint16_t decompressedSize = *compressed++;

        std::unique_ptr<uint16_t[]> decompressed(new uint16_t[decompressedSize / 2]);
        UncompressCarmack(compressed, decompressed.get(), decompressedSize);

        uint16_t expandedSize = decompressed[0];
        assert(level.Data.capacity() * 2 == expandedSize);
        ExpandRLEW(decompressed.get() + 1, level.Data.data(), expandedSize);

        _levels.push_back(level);
        ++offsets;
    }

    return hr;
}

HRESULT Renderer::LoadWolf3DTextures()
{
    size_t size;
    std::unique_ptr<uint8_t[]> buffer;
    wchar_t filename[1024];
    swprintf_s(filename, L"%s\\vswap.wl6", _contentRoot.c_str());
    HRESULT hr = LoadFile(filename, &buffer, &size);
    if (FAILED(hr))
    {
        return hr;
    }

    VSwapHeader* header = (VSwapHeader*)buffer.get();
    uint8_t* data = buffer.get();

    // Walls are uncompressed back to back, starting 4k in
    data += 4096;

    D3D11_TEXTURE2D_DESC td = {};
    td.ArraySize = header->SpritesOffset;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.Height = 64;
    td.MipLevels = 1;
    td.SampleDesc.Count = 1;
    td.Width = 64;
    td.Usage = D3D11_USAGE_DEFAULT;

    ComPtr<ID3D11Texture2D> texture;
    hr = _device->CreateTexture2D(&td, nullptr, &texture);
    if (FAILED(hr))
    {
        return hr;
    }

    std::unique_ptr<uint32_t[]> pixels(new uint32_t[4096]);
    for (uint32_t i = 0; i < header->SpritesOffset; ++i)
    {
        uint32_t* p = pixels.get();
        for (uint32_t y = 0; y < 64; ++y)
        {
            for (uint32_t x = 0; x < 64; ++x)
            {
                PalEntry entry = Wolf3DPalette[data[x * 64 + y]];
                *p++ = 0xFF000000 |
                       (entry.B << 16) |
                       (entry.G << 8) |
                       entry.R;
            }
        }

        _context->UpdateSubresource(texture.Get(), i, nullptr, pixels.get(), 64 * sizeof(uint32_t), 4096 * sizeof(uint32_t));

        data += 4096;
    }

    hr = _device->CreateShaderResourceView(texture.Get(), nullptr, &_textures);

    return S_OK;
}
