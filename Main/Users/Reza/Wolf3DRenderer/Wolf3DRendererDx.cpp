#include "precomp.h"
#include "Wolf3DRenderer.h"
#include "WL6.h"
#include "ClearScreenVS.h"
#include "ClearScreenPS.h"
#include "Draw3DVS.h"
#include "Draw3DPS.h"
#include "DrawSpritesVS.h"
#include "DrawSpritesPS.h"

class Wolf3DRendererDx : public IWolf3DRenderer
{
public:
    static HRESULT CreateInstance(_In_ ID3D11Device* device, _In_ ID3D11RenderTargetView* renderTarget, _Outptr_ IWolf3DRenderer** renderer);

    //
    // IWolf3DRenderer
    //
    HRESULT LoadTextures(const uint8_t* vswapData, uint32_t size) override;
    HRESULT SetColors(const XMFLOAT3& ceiling, const XMFLOAT3& floor) override;
    HRESULT SetWallLayer(uint32_t width, uint32_t height, _In_count_(width * height) const uint16_t* uncompressedWallLayer) override;
    void DrawSprite3D(const XMFLOAT2& positionXZ, uint32_t spriteIndex) override;
    HRESULT Render(const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const RECT& viewport) override;

private:
    Wolf3DRendererDx(_In_ ID3D11Device* device, _In_ ID3D11RenderTargetView* renderTarget);
    Wolf3DRendererDx(const Wolf3DRendererDx&);
    Wolf3DRendererDx& operator= (const Wolf3DRendererDx&);

    HRESULT Initialize();
    HRESULT InitializeClearScreen();
    HRESULT InitializeDraw3D();
    HRESULT InitializeDrawSprites();

private:
    struct ClearVertex
    {
        XMFLOAT2 Position;
    };

    struct ClearConstantsColors
    {
        XMFLOAT4 CeilingColor;
        XMFLOAT4 FloorColor;
    };

    struct ClearConstantsHeight
    {
        uint32_t ViewTop;
        uint32_t ViewHeight;
        XMFLOAT2 Padding;
    };

    struct Draw3DVertex
    {
        XMFLOAT3 Position;
        XMFLOAT3 Normal;
        XMFLOAT2 TexCoord;
    };

    struct Draw3DInstance
    {
        uint32_t TexIndex;
        uint32_t CellIndex;
    };

    struct Draw3DConstants
    {
        XMFLOAT4X4 View;
        XMFLOAT4X4 Projection;
    };

    struct DrawSpriteVertex
    {
        XMFLOAT2 Position;
        XMFLOAT2 TexCoord;
    };

    struct DrawSpriteInstance
    {
        XMFLOAT2 WorldPosition;
        uint32_t TexIndex;
    };

    struct DrawSpriteConstants
    {
        XMFLOAT4X4 View;
        XMFLOAT4X4 Projection;
    };

private:
    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11DeviceContext> _context;
    ComPtr<ID3D11RenderTargetView> _renderTargetView;
    ComPtr<ID3D11DepthStencilView> _depthStencilView;
    ComPtr<ID3D11SamplerState> _sampler;

    //
    // Clearing top/bottom
    //
    ComPtr<ID3D11InputLayout> _inputLayoutClear;
    ComPtr<ID3D11VertexShader> _vertexShaderClear;
    ComPtr<ID3D11PixelShader> _pixelShaderClear;
    ComPtr<ID3D11Buffer> _constantBufferClearColors;
    ComPtr<ID3D11Buffer> _constantBufferClearHeight;
    ComPtr<ID3D11Buffer> _vertexBufferClear;

    //
    // Draw 3D view
    //
    ComPtr<ID3D11InputLayout> _inputLayout3D;
    ComPtr<ID3D11VertexShader> _vertexShader3D;
    ComPtr<ID3D11PixelShader> _pixelShader3D;
    ComPtr<ID3D11Buffer> _constantBuffer3D;
    ComPtr<ID3D11Buffer> _vertexBuffer3D;
    ComPtr<ID3D11Buffer> _instanceBuffer3D;
    ComPtr<ID3D11Buffer> _indexBuffer3D;
    ComPtr<ID3D11ShaderResourceView> _wallTextures;
    uint32_t _numInstances;

    //
    // Draw 2D sprites as billboards
    //
    ComPtr<ID3D11InputLayout> _inputLayoutSprites;
    ComPtr<ID3D11VertexShader> _vertexShaderSprites;
    ComPtr<ID3D11PixelShader> _pixelShaderSprites;
    ComPtr<ID3D11Buffer> _constantBufferSprites;
    ComPtr<ID3D11Buffer> _vertexBufferSprites;
    ComPtr<ID3D11Buffer> _instanceBufferSprites;
    ComPtr<ID3D11Buffer> _indexBufferSprites;
    ComPtr<ID3D11ShaderResourceView> _spriteTextures;
    static const uint32_t MaxSpriteInstances = 512;
    std::unique_ptr<DrawSpriteInstance[]> _spriteInstances;
    uint32_t _numInstancesSprites;
};

//
// Public methods
//

_Use_decl_annotations_
HRESULT CreateWolf3DRendererDx(ID3D11Device* device, ID3D11RenderTargetView* renderTarget, IWolf3DRenderer** renderer)
{
    return Wolf3DRendererDx::CreateInstance(device, renderTarget, renderer);
}

_Use_decl_annotations_
HRESULT Wolf3DRendererDx::CreateInstance(ID3D11Device* device, ID3D11RenderTargetView* renderTarget, IWolf3DRenderer** renderer)
{
    if (!device || !renderTarget || !renderer)
    {
        assert(false);
        return E_INVALIDARG;
    }

    *renderer = nullptr;

    std::unique_ptr<Wolf3DRendererDx> newRenderer(new (std::nothrow) Wolf3DRendererDx(device, renderTarget));
    if (!newRenderer)
    {
        return E_OUTOFMEMORY;
    }

    HRESULT hr = newRenderer->Initialize();
    if (SUCCEEDED(hr))
    {
        *renderer = newRenderer.release();
    }

    return hr;
}

_Use_decl_annotations_
Wolf3DRendererDx::Wolf3DRendererDx(ID3D11Device* device, ID3D11RenderTargetView* renderTarget) :
    _device(device), _renderTargetView(renderTarget), _numInstances(0), _numInstancesSprites(0)
{
    _device->GetImmediateContext(&_context);
}

HRESULT Wolf3DRendererDx::LoadTextures(const uint8_t* vswapData, uint32_t size)
{
    UNREFERENCED_PARAMETER(size);

    VSwapHeader* header = (VSwapHeader*)vswapData;
    const uint8_t* data = vswapData;

    // Read in sprite offsets
    std::vector<uint32_t> spriteOffsets;
    data += sizeof(VSwapHeader) + 4 * header->SpritesOffset;

    uint32_t numSprites = header->SoundsOffset - header->SpritesOffset;

    for (uint32_t i = 0; i < numSprites; ++i)
    {
        spriteOffsets.push_back(*((uint32_t*)data));
        data += 4;
    }

    // Walls are uncompressed back to back, starting 4k in
    data = vswapData + 4096;

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
    HRESULT hr = _device->CreateTexture2D(&td, nullptr, &texture);
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
                *p++ = GetWolf3DColor(data[x * 64 + y]);
            }
        }

        _context->UpdateSubresource(texture.Get(), i, nullptr, pixels.get(), 64 * sizeof(uint32_t), 4096 * sizeof(uint32_t));

        data += 4096;
    }

    hr = _device->CreateShaderResourceView(texture.Get(), nullptr, &_wallTextures);

    //
    // Next is the sprites
    //
    td.ArraySize = numSprites;

    hr = _device->CreateTexture2D(&td, nullptr, &texture);
    if (FAILED(hr))
    {
        return hr;
    }

    for (uint32_t i = 0; i < numSprites; ++i)
    {
        SpriteHeader* sprite = (SpriteHeader*)(vswapData + spriteOffsets[i]);

        // Sprites are compressed into spans across to help with transparency

        // First, fill pixels with all transparent
        for (uint32_t j = 0; j < 64 * 64; ++j)
        {
            pixels[j] = 0x00000000;
        }

        uint8_t* spriteData = (uint8_t*)sprite;
        uint16_t* offsets = sprite->Offsets;
        for (uint16_t sx = sprite->Left; sx <= sprite->Right; ++sx)
        {
            uint16_t* linecmds = (uint16_t*)(spriteData + *offsets++);
            for (; *linecmds; linecmds += 3)
            {
                uint32_t s = linecmds[2] / 2 + linecmds[1];
                for (int32_t y=linecmds[2] / 2; y < linecmds[0] / 2; ++y, ++s)
                {
                    pixels[y * 64 + sx] = GetWolf3DColor(spriteData[s]);
                }
            }
        }

        _context->UpdateSubresource(texture.Get(), i, nullptr, pixels.get(), 64 * sizeof(uint32_t), 4096 * sizeof(uint32_t));
    }

    hr = _device->CreateShaderResourceView(texture.Get(), nullptr, &_spriteTextures);

    return S_OK;
}

HRESULT Wolf3DRendererDx::SetColors(const XMFLOAT3& ceiling, const XMFLOAT3& floor)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = _context->Map(_constantBufferClearColors.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    if (FAILED(hr))
    {
        return hr;
    }

    ClearConstantsColors* constants = (ClearConstantsColors*)mapped.pData;
    constants->CeilingColor = XMFLOAT4(ceiling.x, ceiling.y, ceiling.z, 1);
    constants->FloorColor = XMFLOAT4(floor.x, floor.y, floor.z, 1);

    _context->Unmap(_constantBufferClearColors.Get(), 0);

    return hr;
}

_Use_decl_annotations_
HRESULT Wolf3DRendererDx::SetWallLayer(uint32_t width, uint32_t height, const uint16_t* uncompressedWallLayer)
{
    _numInstances = 0;
    _instanceBuffer3D.Reset();

    static const uint32_t MaxWalls = 98;

    std::vector<Draw3DInstance> instances;
    for (uint32_t i = 0; i < width * height; ++i)
    {
        // Find texture for wall Id
        uint32_t id = (uncompressedWallLayer[i] -1 ) * 2 + 1;
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

void Wolf3DRendererDx::DrawSprite3D(const XMFLOAT2& positionXZ, uint32_t spriteIndex)
{
    if (_numInstancesSprites < MaxSpriteInstances)
    {
        _spriteInstances[_numInstancesSprites].WorldPosition = positionXZ;
        _spriteInstances[_numInstancesSprites].TexIndex = spriteIndex;
        ++_numInstancesSprites;
    }
}

HRESULT Wolf3DRendererDx::Render(const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const RECT& viewport)
{
    D3D11_VIEWPORT vp = {};
    vp.TopLeftX = (float)viewport.left;
    vp.TopLeftY = (float)viewport.top;
    vp.Width = (float)(viewport.right - viewport.left);
    vp.Height = (float)(viewport.bottom - viewport.top);
    vp.MaxDepth = 1.0f;
    _context->RSSetViewports(1, &vp);

    _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //
    // Update view height constant for clearing
    //
    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = _context->Map(_constantBufferClearHeight.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    if (FAILED(hr))
    {
        return hr;
    }

    ClearConstantsHeight* constants = (ClearConstantsHeight*)mapped.pData;
    constants->ViewTop = viewport.top;
    constants->ViewHeight = (viewport.bottom - viewport.top);

    _context->Unmap(_constantBufferClearHeight.Get(), 0);

    //
    // Set up and submit draw for clearing ceiling & floor colors
    //
    static const uint32_t clearStride = sizeof(ClearVertex);
    static const uint32_t clearOffset = 0;

    ID3D11Buffer* clearConstantBuffers[] = { _constantBufferClearColors.Get(), _constantBufferClearHeight.Get() };

    // disable depth buffer for clear pass
    _context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
    _context->IASetInputLayout(_inputLayoutClear.Get());
    _context->IASetVertexBuffers(0, 1, _vertexBufferClear.GetAddressOf(), &clearStride, &clearOffset);
    _context->VSSetShader(_vertexShaderClear.Get(), nullptr, 0);
    _context->PSSetShader(_pixelShaderClear.Get(), nullptr, 0);
    _context->PSSetConstantBuffers(0, _countof(clearConstantBuffers), clearConstantBuffers);
    _context->Draw(6, 0);

    if (_numInstances > 0)
    {
        static const uint32_t strides3D[] = { sizeof(Draw3DVertex), sizeof(Draw3DInstance) };
        static const uint32_t offsets3D[] = { 0, 0 };

        //
        // Set up for 3D pass
        //
        _context->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
        _context->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

        ID3D11Buffer* buffers[] = { _vertexBuffer3D.Get(), _instanceBuffer3D.Get() };
        _context->IASetInputLayout(_inputLayout3D.Get());
        _context->IASetVertexBuffers(0, _countof(buffers), buffers, strides3D, offsets3D);
        _context->IASetIndexBuffer(_indexBuffer3D.Get(), DXGI_FORMAT_R32_UINT, 0);

        hr = _context->Map(_constantBuffer3D.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        if (FAILED(hr))
        {
            return hr;
        }

        Draw3DConstants* constants = (Draw3DConstants*)mapped.pData;
        constants->View = view;
        constants->Projection = projection;

        _context->Unmap(_constantBuffer3D.Get(), 0);

        _context->VSSetShader(_vertexShader3D.Get(), nullptr, 0);
        _context->VSSetConstantBuffers(0, 1, _constantBuffer3D.GetAddressOf());

        _context->PSSetShader(_pixelShader3D.Get(), nullptr, 0);
        _context->PSSetShaderResources(0, 1, _wallTextures.GetAddressOf());
        _context->PSSetSamplers(0, 1, _sampler.GetAddressOf());

        _context->DrawIndexedInstanced(24, _numInstances, 0, 0, 0);
    }

    if (_numInstancesSprites > 0)
    {
        static const uint32_t stridesSprites[] = { sizeof(DrawSpriteVertex), sizeof(DrawSpriteInstance) };
        static const uint32_t offsetsSprites[] = { 0, 0 };

        //
        // Set up for Sprite pass
        //

        D3D11_BOX box = {};
        box.right = sizeof(DrawSpriteInstance) * _numInstancesSprites;
        box.bottom = 1;
        box.back = 1;

        _context->UpdateSubresource(_instanceBufferSprites.Get(), 0, &box, _spriteInstances.get(), box.right, 0);

        ID3D11Buffer* buffers[] = { _vertexBufferSprites.Get(), _instanceBufferSprites.Get() };
        _context->IASetInputLayout(_inputLayoutSprites.Get());
        _context->IASetVertexBuffers(0, _countof(buffers), buffers, stridesSprites, offsetsSprites);
        _context->IASetIndexBuffer(_indexBufferSprites.Get(), DXGI_FORMAT_R32_UINT, 0);

        hr = _context->Map(_constantBufferSprites.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        if (FAILED(hr))
        {
            return hr;
        }

        DrawSpriteConstants* constants = (DrawSpriteConstants*)mapped.pData;
        constants->View = view;
        constants->Projection = projection;

        _context->Unmap(_constantBufferSprites.Get(), 0);

        _context->VSSetShader(_vertexShaderSprites.Get(), nullptr, 0);
        _context->VSSetConstantBuffers(0, 1, _constantBufferSprites.GetAddressOf());

        _context->PSSetShader(_pixelShaderSprites.Get(), nullptr, 0);
        _context->PSSetShaderResources(0, 1, _spriteTextures.GetAddressOf());
        _context->PSSetSamplers(0, 1, _sampler.GetAddressOf());

        _context->DrawIndexedInstanced(6, _numInstancesSprites, 0, 0, 0);

        _numInstancesSprites = 0;
    }

    return hr;
}

//
// Private methods
//

HRESULT Wolf3DRendererDx::Initialize()
{
    ComPtr<ID3D11Resource> resource;
    _renderTargetView->GetResource(&resource);

    ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = resource.As(&texture);
    if (FAILED(hr))
    {
        return hr;
    }

    //
    // Create depth buffer
    //
    D3D11_TEXTURE2D_DESC td = {};
    texture->GetDesc(&td);
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.Format = DXGI_FORMAT_D32_FLOAT;

    hr = _device->CreateTexture2D(&td, nullptr, texture.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        return hr;
    }

    hr = _device->CreateDepthStencilView(texture.Get(), nullptr, &_depthStencilView);
    if (FAILED(hr))
    {
        return hr;
    }

    //
    // Create point sampler
    //
    D3D11_SAMPLER_DESC sd = {};
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

    hr = _device->CreateSamplerState(&sd, &_sampler);
    if (FAILED(hr))
    {
        return hr;
    }

    //
    // Initialize stuff needed for ceiling/floor coloring
    //
    hr = InitializeClearScreen();
    if (FAILED(hr))
    {
        return hr;
    }

    //
    // Initialize stuff needed for 3D rendering
    //
    hr = InitializeDraw3D();
    if (FAILED(hr))
    {
        return hr;
    }

    //
    // Initialize stuff needed for rendering billboarded sprites
    //
    hr = InitializeDrawSprites();
    if (FAILED(hr))
    {
        return hr;
    }

    return hr;
}

HRESULT Wolf3DRendererDx::InitializeClearScreen()
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(ClearConstantsColors);
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.StructureByteStride = bd.ByteWidth;
    bd.Usage = D3D11_USAGE_DYNAMIC;

    ClearConstantsColors constants;
    constants.CeilingColor = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    constants.FloorColor = XMFLOAT4(0.4f, 0.4f, 0.45f, 1.0f);

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = &constants;
    init.SysMemPitch = sizeof(constants);

    HRESULT hr = _device->CreateBuffer(&bd, &init, &_constantBufferClearColors);
    if (FAILED(hr))
    {
        return hr;
    }

    bd.ByteWidth = sizeof(ClearConstantsHeight);
    bd.StructureByteStride = bd.ByteWidth;

    hr = _device->CreateBuffer(&bd, nullptr, &_constantBufferClearHeight);
    if (FAILED(hr))
    {
        return hr;
    }


    hr = _device->CreateVertexShader(g_ClearScreenVS, sizeof(g_ClearScreenVS), nullptr, &_vertexShaderClear);
    if (FAILED(hr))
    {
        return hr;
    }

    D3D11_INPUT_ELEMENT_DESC elems[1] = {};
    elems[0].Format = DXGI_FORMAT_R32G32_FLOAT;
    elems[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elems[0].SemanticName = "POSITION";

    hr = _device->CreateInputLayout(elems, _countof(elems), g_ClearScreenVS, sizeof(g_ClearScreenVS), &_inputLayoutClear);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = _device->CreatePixelShader(g_ClearScreenPS, sizeof(g_ClearScreenPS), nullptr, &_pixelShaderClear);
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

HRESULT Wolf3DRendererDx::InitializeDraw3D()
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

    hr = _device->CreateVertexShader(g_Draw3DVS, sizeof(g_Draw3DVS), nullptr, &_vertexShader3D);
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

    hr = _device->CreateInputLayout(elems, _countof(elems), g_Draw3DVS, sizeof(g_Draw3DVS), &_inputLayout3D);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = _device->CreatePixelShader(g_Draw3DPS, sizeof(g_Draw3DPS), nullptr, &_pixelShader3D);
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

    return hr;
}

HRESULT Wolf3DRendererDx::InitializeDrawSprites()
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(DrawSpriteConstants);
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.StructureByteStride = bd.ByteWidth;
    bd.Usage = D3D11_USAGE_DYNAMIC;

    HRESULT hr = _device->CreateBuffer(&bd, nullptr, &_constantBufferSprites);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = _device->CreateVertexShader(g_DrawSpritesVS, sizeof(g_DrawSpritesVS), nullptr, &_vertexShaderSprites);
    if (FAILED(hr))
    {
        return hr;
    }

    D3D11_INPUT_ELEMENT_DESC elems[4] = {};
    elems[0].Format = DXGI_FORMAT_R32G32_FLOAT;
    elems[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elems[0].SemanticName = "POSITION";

    elems[1].AlignedByteOffset = sizeof(XMFLOAT2);
    elems[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    elems[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elems[1].SemanticName = "TEXCOORD";

    elems[2].Format = DXGI_FORMAT_R32G32_FLOAT;
    elems[2].InputSlot = 1;
    elems[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    elems[2].InstanceDataStepRate = 1;
    elems[2].SemanticName = "POSITION";
    elems[2].SemanticIndex = 1;

    elems[3].AlignedByteOffset = sizeof(XMFLOAT2);
    elems[3].Format = DXGI_FORMAT_R32_UINT;
    elems[3].InputSlot = 1;
    elems[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
    elems[3].InstanceDataStepRate = 1;
    elems[3].SemanticName = "TEXCOORD";
    elems[3].SemanticIndex = 1;

    hr = _device->CreateInputLayout(elems, _countof(elems), g_DrawSpritesVS, sizeof(g_DrawSpritesVS), &_inputLayoutSprites);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = _device->CreatePixelShader(g_DrawSpritesPS, sizeof(g_DrawSpritesPS), nullptr, &_pixelShaderSprites);
    if (FAILED(hr))
    {
        return hr;
    }

    // A simple quad
    DrawSpriteVertex vertices[] = 
    {
        { XMFLOAT2(-0.5f, 0.5f), XMFLOAT2(0, 0) },
        { XMFLOAT2(0.5f, 0.5f), XMFLOAT2(1, 0) },
        { XMFLOAT2(0.5f, -0.5f), XMFLOAT2(1, 1) },
        { XMFLOAT2(-0.5f, -0.5f), XMFLOAT2(0, 1) },
    };

    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(vertices);
    bd.StructureByteStride = sizeof(vertices[0]);
    bd.CPUAccessFlags = 0;
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = vertices;
    init.SysMemPitch = bd.ByteWidth;

    hr = _device->CreateBuffer(&bd, &init, &_vertexBufferSprites);
    if (FAILED(hr))
    {
        return hr;
    }

    uint32_t indices[] =
    {
        0, 1, 2, 0, 2, 3,
    };

    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.ByteWidth = sizeof(indices);
    bd.StructureByteStride = sizeof(indices[0]);

    init.pSysMem = indices;
    init.SysMemPitch = bd.ByteWidth;

    hr = _device->CreateBuffer(&bd, &init, &_indexBufferSprites);
    if (FAILED(hr))
    {
        return hr;
    }

    _spriteInstances.reset(new DrawSpriteInstance[MaxSpriteInstances]);

    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(DrawSpriteInstance) * MaxSpriteInstances;
    bd.StructureByteStride = sizeof(DrawSpriteInstance);

    hr = _device->CreateBuffer(&bd, nullptr, &_instanceBufferSprites);
    if (FAILED(hr))
    {
        return hr;
    }

    return hr;
}
