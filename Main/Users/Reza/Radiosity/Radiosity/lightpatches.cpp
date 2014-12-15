#include "precomp.h"
#include "renderer.h"
#include "lightpatch.h"
#include "camera.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace DirectX;

//==============================================================================

PatchManager::PatchManager(_In_ const ComPtr<ID3D11Device>& device) :
    _device(device), _patchViewSize(128), _patchSizeWorld(0), _mapSizePixels(0)
{
    D3D11_TEXTURE2D_DESC td = {};
    td.ArraySize = 1;
    td.BindFlags = D3D11_BIND_RENDER_TARGET;
    td.Format = DXGI_FORMAT_R32_FLOAT;
    td.Width = _patchViewSize;
    td.Height = _patchViewSize;
    td.MipLevels = 1;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;

    for (uint32_t i = 0; i < _countof(_textures); ++i)
    {
        if (FAILED(device->CreateTexture2D(&td, nullptr, &_textures[i])))
        {
            throw std::exception();
        }

        if (FAILED(device->CreateRenderTargetView(_textures[i].Get(), nullptr, &_rtv[i])))
        {
            throw std::exception();
        }
    }

    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.Format = DXGI_FORMAT_D32_FLOAT;

    ComPtr<ID3D11Texture2D> texture;
    if (FAILED(device->CreateTexture2D(&td, nullptr, &texture)))
    {
        throw std::exception();
    }

    if (FAILED(device->CreateDepthStencilView(texture.Get(), nullptr, &_depthStencil)))
    {
        throw std::exception();
    }

    td.Width = 2 * _patchViewSize;
    td.Height = 2 * _patchViewSize;
    td.BindFlags = 0;
    td.Format = DXGI_FORMAT_R32_FLOAT;
    td.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    td.Usage = D3D11_USAGE_STAGING;

    uint32_t count = td.Width * td.Height;
    std::unique_ptr<float[]> nullPatches(new float[count]);
    memset(nullPatches.get(), 0, count * sizeof(float));

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = nullPatches.get();
    init.SysMemPitch = td.Width * sizeof(float);

    if (FAILED(_device->CreateTexture2D(&td, &init, &_patchView)))
    {
        throw std::exception();
    }

    _device->GetImmediateContext(&_context);

    BuildMuliplierMap();
}

void PatchManager::BeginPatchMap(_In_ float patchSizeWorld, _In_ uint32_t mapSizePixels)
{
    _patchSizeWorld = patchSizeWorld;
    _mapSizePixels = mapSizePixels;

    _patchBuilder.reset(new PatchBuilder);
    _patchBuilder->currentHeight = _patchBuilder->currentX = _patchBuilder->currentY = 0;

    _patchBuilder->patchMap.reset(new uint32_t[mapSizePixels * mapSizePixels]);
    memset(_patchBuilder->patchMap.get(), 0xFFFFFFFF, mapSizePixels * mapSizePixels * sizeof(uint32_t));

    _patches.clear();
    _lightMap = nullptr;
}

void PatchManager::InsertQuad(_Inout_ Quad& quad, _In_ const DirectX::XMFLOAT4X4& world, _In_ const DirectX::XMFLOAT3& emission)
{
    // All quads are assumed to come in with the following ordering:
    // lower left, upper left, upper right, lower right. Or:
    //
    // B---C
    // |   |
    // A---D
    //
    // And tangent is always along B -> C, so u will increase along that
    // bitangent is always from B -> A, so v will increase along that

    XMMATRIX worldMatrix = XMLoadFloat4x4(&world);

    XMVECTOR A = XMVector3Transform(XMLoadFloat3(&quad.A.Position), worldMatrix);
    XMVECTOR B = XMVector3Transform(XMLoadFloat3(&quad.B.Position), worldMatrix);
    XMVECTOR C = XMVector3Transform(XMLoadFloat3(&quad.C.Position), worldMatrix);
    XMVECTOR D = XMVector3Transform(XMLoadFloat3(&quad.D.Position), worldMatrix);

    XMVECTOR tangent = XMVector3Normalize(XMVectorSubtract(C, B));
    XMVECTOR bitangent = XMVector3Normalize(XMVectorSubtract(A, B));

    float uMin = XMVectorGetX(XMVectorMin(XMVector3Dot(A, tangent), XMVector3Dot(B, tangent)));
    float uMax = XMVectorGetX(XMVectorMax(XMVector3Dot(C, tangent), XMVector3Dot(D, tangent)));

    float vMin = XMVectorGetX(XMVectorMin(XMVector3Dot(B, bitangent), XMVector3Dot(C, bitangent)));
    float vMax = XMVectorGetX(XMVectorMax(XMVector3Dot(A, bitangent), XMVector3Dot(D, bitangent)));

    float width = uMax - uMin;
    float height = vMax - vMin;

    uint32_t widthInPatches = static_cast<uint32_t>(width / _patchSizeWorld + 0.5f);
    uint32_t heightInPatches = static_cast<uint32_t>(height / _patchSizeWorld + 0.5f);

    if (_patchBuilder->currentX + widthInPatches > _mapSizePixels)
    {
        _patchBuilder->currentX = 0;
        _patchBuilder->currentY = _patchBuilder->currentHeight + 1;
    }

    RECT rect = {};
    rect.left = _patchBuilder->currentX;
    rect.right = _patchBuilder->currentX + widthInPatches;
    rect.top = _patchBuilder->currentY;
    rect.bottom = _patchBuilder->currentY + heightInPatches;

    Patch patch = {};
    patch.reflectance = 0.2f;
    patch.emission = emission.x;
    XMStoreFloat3(&patch.normal, XMVector3Normalize(XMVector3Cross(tangent, bitangent)));
    XMStoreFloat3(&patch.up, XMVector3Normalize(XMVectorNegate(bitangent)));

    for (uint32_t y = 0; y < heightInPatches; ++y)
    {
        float deltaY = static_cast<float>(y) / heightInPatches;
        XMVECTOR pos = XMVectorAdd(B, XMVectorScale(bitangent, deltaY * height));

        for (uint32_t x = 0; x < widthInPatches; ++x)
        {
            float deltaX = static_cast<float>(x) / widthInPatches;
            XMStoreFloat3(&patch.position, XMVectorAdd(pos, XMVectorScale(tangent, deltaX * width)));

            _patchBuilder->patchMap[(rect.top + y) * _mapSizePixels + (rect.left + x)] = static_cast<uint32_t>(_patches.size());

            patch.x = rect.left + x;
            patch.y = rect.top + y;
            _patches.push_back(patch);
        }
    }

    _patchBuilder->rects.push_back(rect);

    _patchBuilder->currentX = rect.right;

    if (rect.bottom > static_cast<LONG>(_patchBuilder->currentHeight))
    {
        _patchBuilder->currentHeight = rect.bottom;
    }

    // compute uv now
    quad.A.LightUV.x = static_cast<float>(rect.left) / _mapSizePixels;
    quad.A.LightUV.y = static_cast<float>(rect.bottom) / _mapSizePixels;
    quad.B.LightUV.x = static_cast<float>(rect.left) / _mapSizePixels;
    quad.B.LightUV.y = static_cast<float>(rect.top) / _mapSizePixels;
    quad.C.LightUV.x = static_cast<float>(rect.right) / _mapSizePixels;
    quad.C.LightUV.y = static_cast<float>(rect.top) / _mapSizePixels;
    quad.D.LightUV.x = static_cast<float>(rect.right) / _mapSizePixels;
    quad.D.LightUV.y = static_cast<float>(rect.bottom) / _mapSizePixels;
}

void PatchManager::EndPatchMap()
{
    D3D11_TEXTURE2D_DESC td = {};
    td.ArraySize = 1;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.Format = DXGI_FORMAT_R32_FLOAT;
    td.Width = _mapSizePixels;
    td.Height = _mapSizePixels;
    td.MipLevels = 1;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = _patchBuilder->patchMap.get();
    init.SysMemPitch = sizeof(float) * _mapSizePixels;

    if (FAILED(_device->CreateTexture2D(&td, nullptr, &_lightMap)))
    {
        throw std::exception();
    }

    _lightMapData.reset(new float[_mapSizePixels * _mapSizePixels]);
}

void PatchManager::GeneratePatchViews(_In_ Renderer* renderer)
{
    ScratchImage result;

    // Initialize all patches to their emission values
    ZeroMemory(_lightMapData.get(), _mapSizePixels * _mapSizePixels * sizeof(float));

    Patch* patch = _patches.data();
    for (size_t i = 0; i < _patches.size(); ++i, ++patch)
    {
        _lightMapData.get()[patch->y * _mapSizePixels + patch->x] = patch->emission;
    }

    // Initialize the lightmap for use
    _context->UpdateSubresource(_lightMap.Get(), 0, nullptr, _lightMapData.get(), _mapSizePixels * sizeof(float), 0);

    CaptureTexture(_device.Get(), _context.Get(), _lightMap.Get(), result);
    if (FAILED(SaveToDDSFile(result.GetImage(0, 0, 0), 1, result.GetMetadata(), 0, L"InitialLightmap.dds")))
    {
        throw std::exception();
    }
    // passes
    for (uint32_t pass = 0; pass < 20; ++pass)
    {
        // process patches
        Patch* patch = _patches.data();
        for (size_t i = 0; i < _patches.size(); ++i, ++patch)
        {
            RenderPatchViews(renderer, patch);
            ComputeLight(patch);
        }

        // Update the light map with the new values
        _context->UpdateSubresource(_lightMap.Get(), 0, nullptr, _lightMapData.get(), _mapSizePixels * sizeof(float), 0);
    }

    CaptureTexture(_device.Get(), _context.Get(), _lightMap.Get(), result);
    if (FAILED(SaveToDDSFile(result.GetImage(0, 0, 0), 1, result.GetMetadata(), 0, L"FinalLightmap.dds")))
    {
        throw std::exception();
    }
}

const Microsoft::WRL::ComPtr<ID3D11Texture2D>& PatchManager::GetLightMap() const
{
    return _lightMap;
}

//==============================================================================

void PatchManager::BuildMuliplierMap()
{
    // The map is the blown out view, with forward view in the center, and the other
    // 4 surrounding faces unwrapped around it. This means its twice as large as patch
    // view size in each dimension
    uint32_t mapSize = _patchViewSize * 2;
    uint32_t quarter = _patchViewSize / 2;
    uint32_t threeQuarter = _patchViewSize + quarter;
    _multiplierMap.reset(new double[mapSize * mapSize]);

    double* map = _multiplierMap.get();

    // There are 2 submaps being built, then combined into the multiplier map.

    // the first submap is the cosine table. This accounts for the distortion of objects
    // near the edge of view, and is the dot of the vector to the pixel & the view direction
    // of the camera that side maps to.

    // the second submap is the lambert table. This accounts for the standard N dot L
    // lighting. These are the dot of the vector to the pixel & the normal of the surface.
    // the normal of the surface is the forward vector of the forward view

    // to build the final map in a single pass, we need to compute both submap values per
    // pixel and then multiply them. To do that, we need some info to compute the pixel vector,
    // and the view vectors. We also need the distance to the plane of each view, which is equivalent
    // to the 'quarter' value from above.
    float dist = static_cast<float>(quarter);
    XMVECTOR forward = XMVectorSet(0, 0, 1, 0);
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);
    XMVECTOR down = XMVectorSet(0, -1, 0, 0);
    XMVECTOR left = XMVectorSet(-1, 0, 0, 0);
    XMVECTOR right = XMVectorSet(1, 0, 0, 0);
    XMVECTOR targetView;
    XMVECTOR v;

    for (uint32_t y = 0; y < mapSize; ++y)
    {
        for (uint32_t x = 0; x< mapSize; ++x, ++map)
        {
            // which face are we in? gather the data in the right space
            if (x >= quarter && x < threeQuarter)
            {
                // either up, forward, or down
                if (y < quarter)
                {
                    // up
                    targetView = up;
                    v = XMVector3Normalize(XMVectorSet(x - 2 * dist, dist, static_cast<float>(y), 0));
                }
                else if (y < threeQuarter)
                {
                    // forward
                    targetView = forward;
                    v = XMVector3Normalize(XMVectorSet(x - 2 * dist, 2 * dist - y, dist, 0));
                }
                else
                {
                    // down
                    targetView = down;
                    v = XMVector3Normalize(XMVectorSet(x - 2 * dist, -dist, 4 * dist - y, 0));
                }
            }
            else if (y >= quarter && y < threeQuarter)
            {
                // either left or right
                if (x < quarter)
                {
                    // left
                    targetView = left;
                    v = XMVector3Normalize(XMVectorSet(-dist, 2 * dist - y, static_cast<float>(x) , 0));
                }
                else
                {
                    // right
                    targetView = right;
                    v = XMVector3Normalize(XMVectorSet(dist, 2 * dist -y, 4 * dist - x, 0));
                }
            }
            else
            {
                // one of the corners (holes)
                *map = 0.0f;
                continue;
            }

            float cosine = XMVectorGetX(XMVector3Dot(targetView, v));
            float lambert = XMVectorGetX(XMVector3Dot(forward, v));
            *map = static_cast<double>(cosine) * static_cast<double>(lambert);
        }
    }

    // normalization

    double total = 0.0;
    map = _multiplierMap.get();
    for (uint32_t i = 0; i < mapSize * mapSize; ++i, ++map)
    {
        total += *map;
    }

    map = _multiplierMap.get();
    for (uint32_t i = 0; i < mapSize * mapSize; ++i, ++map)
    {
        *map = *map / total;
    }
}

void PatchManager::RenderPatchViews(_In_ Renderer* renderer, _In_ Patch* patch)
{
    XMVECTOR position = XMLoadFloat3(&patch->position);
    XMVECTOR normal = XMLoadFloat3(&patch->normal);
    XMVECTOR up = XMLoadFloat3(&patch->up);
    XMVECTOR left = XMVector3Cross(normal, up);

    XMMATRIX views[5] = 
    {
        // forward
        XMMatrixLookAtLH(position, XMVectorAdd(position, normal), up),
        // up
        XMMatrixLookAtLH(position, XMVectorAdd(position, up), XMVectorNegate(normal)),
        // down
        XMMatrixLookAtLH(position, XMVectorAdd(position, XMVectorNegate(up)), normal),
        // left
        XMMatrixLookAtLH(position, XMVectorAdd(position, left), up),
        // right
        XMMatrixLookAtLH(position, XMVectorAdd(position, XMVectorNegate(left)), up)
    };

    XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), 1.0f, 0.1f, 1000.0f);

    const float clear[4] = { 0, 0, 0, 0 };
    for (uint32_t i = 0; i < _countof(views); ++i)
    {
        _context->ClearRenderTargetView(_rtv[i].Get(), clear);
        _context->ClearDepthStencilView(_depthStencil.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        _context->OMSetRenderTargets(1, _rtv[i].GetAddressOf(), _depthStencil.Get());

        XMStoreFloat4x4(&renderer->_vertexShaderConstants.ViewProjection, XMMatrixMultiply(views[i], proj));
        _context->UpdateSubresource(renderer->_vertexShaderConstantBuffer.Get(), 0, nullptr, &renderer->_vertexShaderConstants, sizeof(renderer->_vertexShaderConstants), 0);

        renderer->DrawScene();

        switch (i)
        {
        case 0: // forward
            _context->CopySubresourceRegion(_patchView.Get(), 0, _patchViewSize / 2, _patchViewSize / 2, 0, _textures[i].Get(), 0, nullptr);
            break;
        case 1: // up
            {
                D3D11_BOX box = {};
                box.left = 0;
                box.right = _patchViewSize;
                box.top = _patchViewSize / 2;
                box.bottom = _patchViewSize;
                box.back = 1;
                _context->CopySubresourceRegion(_patchView.Get(), 0, _patchViewSize / 2, 0, 0, _textures[i].Get(), 0, &box);
            }
            break;
        case 2: // down
            {
                D3D11_BOX box = {};
                box.left = 0;
                box.right = _patchViewSize;
                box.top = 0;
                box.bottom = _patchViewSize / 2;
                box.back = 1;
                _context->CopySubresourceRegion(_patchView.Get(), 0, _patchViewSize / 2, _patchViewSize + (_patchViewSize / 2), 0, _textures[i].Get(), 0, &box);
            }
            break;
        case 3: // left
            {
                D3D11_BOX box = {};
                box.left = _patchViewSize / 2;
                box.right = _patchViewSize;
                box.top = 0;
                box.bottom = _patchViewSize;
                box.back = 1;
                _context->CopySubresourceRegion(_patchView.Get(), 0, 0, _patchViewSize / 2, 0, _textures[i].Get(), 0, &box);
            }
            break;
        case 4: // right
            {
                D3D11_BOX box = {};
                box.left = 0;
                box.right = _patchViewSize / 2;
                box.top = 0;
                box.bottom = _patchViewSize;
                box.back = 1;
                _context->CopySubresourceRegion(_patchView.Get(), 0, _patchViewSize + (_patchViewSize / 2), _patchViewSize / 2, 0, _textures[i].Get(), 0, &box);
            }
            break;
        }
    }

#if 0
    static UINT num = 1;
    static wchar_t filename[MAX_PATH] = {};
    swprintf_s(filename, L"view%d.dds", num++);

    ScratchImage result;
    CaptureTexture(_device.Get(), _context.Get(), _patchView.Get(), result);
    if (FAILED(SaveToDDSFile(result.GetImage(0, 0, 0), 1, result.GetMetadata(), 0, filename)))
    {
        throw std::exception();
    }
#endif
}

void PatchManager::ComputeLight(_In_ Patch* patch)
{
    uint32_t mapSize = _patchViewSize * 2;

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    _context->Map(_patchView.Get(), 0, D3D11_MAP_READ, 0, &mapped);

    float* view = reinterpret_cast<float*>(mapped.pData);
    double* mult = _multiplierMap.get();

    double incident = 0.0;
    for (uint32_t y = 0; y < mapSize; ++y)
    {
        for (uint32_t x = 0; x < mapSize; ++x, ++view, ++mult)
        {
            incident += (*view) * (*mult);
        }
    }

    _context->Unmap(_patchView.Get(), 0);

    _lightMapData.get()[patch->y * _mapSizePixels + patch->x] += static_cast<float>(incident * patch->reflectance + patch->emission);
}

void Renderer::BindPatchPipeline()
{
    // TODO: Move this into the patch manager

    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->VSSetConstantBuffers(0, 1, _vertexShaderConstantBuffer.GetAddressOf());

    _context->PSSetShader(_patchPixelShader.Get(), nullptr, 0);
    _context->PSSetShaderResources(0, 1, _lightMapSRV.GetAddressOf());
    _context->PSSetSamplers(0, 1, _pointSampler.GetAddressOf());

    D3D11_VIEWPORT vp = {};
    vp.Width = 128.0f;
    vp.Height = 128.0f;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

    _context->RSSetViewports(1, &vp);
}

