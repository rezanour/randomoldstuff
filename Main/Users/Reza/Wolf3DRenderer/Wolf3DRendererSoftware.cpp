#include "precomp.h"
#include "Wolf3DRenderer.h"
#include "WL6.h"
#include <math.h>

class Wolf3DRendererSoftware : public IWolf3DRenderer
{
public:
    static HRESULT CreateInstance(uint32_t width, uint32_t height, _In_count_(width * height) uint32_t* surface, _Outptr_ IWolf3DRenderer** renderer);

    //
    // IWolf3DRenderer
    //
    HRESULT LoadTextures(const uint8_t* vswapData, uint32_t size) override;
    HRESULT SetColors(const XMFLOAT3& ceiling, const XMFLOAT3& floor) override;
    HRESULT SetWallLayer(uint32_t width, uint32_t height, _In_count_(width * height) const uint16_t* uncompressedWallLayer) override;
    void DrawSprite3D(const XMFLOAT2& positionXZ, uint32_t spriteIndex) override;
    HRESULT Render(const XMFLOAT4X4&, const XMFLOAT4X4&, const RECT&) override;

private:
    Wolf3DRendererSoftware(uint32_t width, uint32_t height, _In_count_(width * height) uint32_t* surface);
    Wolf3DRendererSoftware(const Wolf3DRendererSoftware&);
    Wolf3DRendererSoftware& operator= (const Wolf3DRendererSoftware&);

    HRESULT Initialize();

    void RasterizeColumn(uint32_t col, const XMFLOAT2& position, const XMFLOAT2& ray);

private:
    int _width;
    int _height;
    uint32_t* _surface;
    const uint16_t* _map;

    uint32_t _ceilingColor;
    uint32_t _floorColor;

    XMFLOAT2 _viewDir;
    float _distToPlane;
};

//
// Public methods
//

_Use_decl_annotations_
HRESULT CreateWolf3DRendererSoftware(uint32_t width, uint32_t height, uint32_t* surface, IWolf3DRenderer** renderer)
{
    return Wolf3DRendererSoftware::CreateInstance(width, height, surface, renderer);
}

_Use_decl_annotations_
HRESULT Wolf3DRendererSoftware::CreateInstance(uint32_t width, uint32_t height, uint32_t* surface, IWolf3DRenderer** renderer)
{
    if (!surface || !renderer)
    {
        assert(false);
        return E_INVALIDARG;
    }

    *renderer = nullptr;

    std::unique_ptr<Wolf3DRendererSoftware> newRenderer(new (std::nothrow) Wolf3DRendererSoftware(width, height, surface));
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
Wolf3DRendererSoftware::Wolf3DRendererSoftware(uint32_t width, uint32_t height, uint32_t* surface) :
    _width((int)width), _height((int)height), _surface(surface), _ceilingColor(0xFFAAAAAA), _floorColor(0xFF555555)
{
}

HRESULT Wolf3DRendererSoftware::LoadTextures(const uint8_t* vswapData, uint32_t size)
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

#if 0
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
        SpriteHeader* sprite = (SpriteHeader*)(buffer.get() + spriteOffsets[i]);

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
#endif
    return S_OK;
}

HRESULT Wolf3DRendererSoftware::SetColors(const XMFLOAT3& ceiling, const XMFLOAT3& floor)
{
    _ceilingColor = 0xFF000000 | 
                    (uint32_t)((uint8_t)(ceiling.z * 256)) << 16 |
                    (uint32_t)((uint8_t)(ceiling.y * 256)) << 8 |
                    (uint8_t)(ceiling.x * 256);

    _floorColor = 0xFF000000 | 
                  (uint32_t)((uint8_t)(floor.z * 256)) << 16 |
                  (uint32_t)((uint8_t)(floor.y * 256)) << 8 |
                  (uint8_t)(floor.x * 256);

    return S_OK;
}

_Use_decl_annotations_
HRESULT Wolf3DRendererSoftware::SetWallLayer(uint32_t width, uint32_t height, const uint16_t* uncompressedWallLayer)
{
    if (width != 64 || height != 64)
    {
        // currently, only support 64x64
        assert(false);
        return E_INVALIDARG;
    }

    _map = uncompressedWallLayer;

    static const uint32_t MaxWalls = 98;

    for (uint32_t i = 0; i < width * height; ++i)
    {
        // Find texture for wall Id
        uint32_t id = (uncompressedWallLayer[i] -1 ) * 2 + 1;
        if (id > 0 && id < MaxWalls)
        {
        }
    }

    return S_OK;
}

void Wolf3DRendererSoftware::DrawSprite3D(const XMFLOAT2& positionXZ, uint32_t spriteIndex)
{
    UNREFERENCED_PARAMETER(positionXZ);
    UNREFERENCED_PARAMETER(spriteIndex);
}

HRESULT Wolf3DRendererSoftware::Render(const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const RECT& viewport)
{
    UNREFERENCED_PARAMETER(viewport);

    // Extract view vector from view matrix and fov from projection matrix
    XMVECTOR det;
    XMMATRIX cameraWorld = XMMatrixInverse(&det, XMLoadFloat4x4(&view));
    XMFLOAT3 position;
    XMStoreFloat3(&position, cameraWorld.r[3]);
    position.x += 32;
    position.z += 32;
    position.z = 64 - position.z;

    XMFLOAT3 viewDir3D;
    XMStoreFloat3(&viewDir3D, cameraWorld.r[2]);
    _viewDir = XMFLOAT2(viewDir3D.x, viewDir3D.z);

    float fov = atan(projection._22);
    _distToPlane = (_width / 2) / tan(fov * 0.5f);
    XMFLOAT2 perpDir(_viewDir.y, -_viewDir.x);
    int32_t halfWidth = _width / 2;
    XMFLOAT2 centerOfPlane(position.x + _viewDir.x * _distToPlane, position.z + _viewDir.y * _distToPlane);

    XMFLOAT2 pos(position.x, position.z);

    // Start with left edge, and cast n rays across the fov, where n == width
    for (int32_t x = 0; x < (int)_width; ++x)
    {
        int32_t n = -halfWidth + x;
        XMFLOAT2 p(centerOfPlane.x + perpDir.x * n, centerOfPlane.y + perpDir.y * n);
        XMFLOAT2 ray(p.x - position.x, p.y - position.y);
        XMStoreFloat2(&ray, XMVector2Normalize(XMLoadFloat2(&ray)));

        RasterizeColumn(x, pos, ray);
    }

    return S_OK;
}

//
// Private methods
//

HRESULT Wolf3DRendererSoftware::Initialize()
{
    return S_OK;
}

void Wolf3DRendererSoftware::RasterizeColumn(uint32_t col, const XMFLOAT2& position, const XMFLOAT2& ray)
{
    UNREFERENCED_PARAMETER(col);

    int cx, cy;                         // cell being tested
    int cxStep = 0, cyStep = 0;         // cell step
    float distX, distY;                 // next nearest point for x and y iterations
    float distXStep = 0, distYStep = 0; // step size of dist in each iteration
    float slope;

    cx = (int)position.x;
    cy = (int)position.y;
    slope = ray.y / ray.x;

    if (ray.x == 0)
    {
        // vertical
        cyStep = (ray.y >= 0) ? -1 : 1;
        distX = FLT_MAX;
        distY = (float)((ray.y >= 0) ? (position.y - cy) : ((cy + 1) - position.y));
        distYStep = (float)cyStep;
    }
    else if (ray.y == 0)
    {
        // horizontal
        cxStep = (ray.x >= 0) ? 1 : -1;
        distY = FLT_MAX;
        distX = (float)((ray.x >= 0) ? ((cx + 1) - position.x) : (position.x - cx));
        distXStep = (float)cxStep;
    }
    else
    {
        if (ray.x > 0)
        {
            cxStep = 1;
            distX = ((cx + 1) - position.x) * slope;
            distXStep = slope;
        }
        else
        {
            cxStep = -1;
            distX = (position.x - cx) * slope;
            distXStep = -slope;
        }

        if (ray.y > 0)
        {
            cyStep = -1;
            distY = (position.y - cy) / slope;
            distYStep = 1.0f / slope;
        }
        else
        {
            cyStep = 1;
            distY = ((cy + 1) - position.y) / slope;
            distYStep = -1.0f / slope;
        }
    }

    float dist = -1;
    for (;;)
    {
        if (distX <= distY)
        {
            cx += cxStep;
            if (cx < 0 || cx >= 64)
            {
                break;
            }

            if (_map[cy * 64 + cx] > 0)
            {
                // hit!
                dist = distX;
                break;
            }

            distX += distXStep;
        }
        else
        {
            cy += cyStep;
            if (cy < 0 || cy >= 64)
            {
                break;
            }

            if (_map[cy * 64 + cx] > 0)
            {
                // hit!
                dist = distY;
                break;
            }

            distY += distYStep;
        }
    }

    int y = 0;

    if (dist > 0)
    {
        float correctedDist = XMVectorGetX(XMVector2Dot(XMVectorScale(XMLoadFloat2(&ray), dist), XMLoadFloat2(&_viewDir)));
        float scaledHeight = 64.0f * _distToPlane / correctedDist;
        int startY = (int)(_height / 2 - scaledHeight * 0.5f);
        int endY = (int)(_height / 2 + scaledHeight * 0.5f);

        if (startY < 0) startY = 0;
        if (startY > _height) startY = _height;
        if (endY < 0) endY = 0;
        if (endY > _height) endY = _height;

        for (; y < startY; ++y)
        {
            _surface[y * _width + col] = _ceilingColor;
        }
        for (; y < endY; ++y)
        {
            _surface[y * _width + col] = 0xFFFF0000;
        }
        for (; y < _height; ++y)
        {
            _surface[y * _width + col] = _floorColor;
        }
    }
    else
    {
        for (; y < _height / 2; ++y)
        {
            _surface[y * _width + col] = _ceilingColor;
        }
        for (; y < _height; ++y)
        {
            _surface[y * _width + col] = _floorColor;
        }
    }
}
