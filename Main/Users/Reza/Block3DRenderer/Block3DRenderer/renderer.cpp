#include "precomp.h"
#include "block3d.h"
#include "renderer.h"
#include "ClearVS.h"
#include "ClearPS.h"
#include "VertexShader.h"
#include "PixelShader.h"

_Use_decl_annotations_
IBlock3DRenderer* CreateBlock3DRenderer(ID3D11Device* device, ID3D11ShaderResourceView* walls, ID3D11ShaderResourceView* sprites, bool useLinearFiltering)
{
    return new Block3DRenderer(device, walls, sprites, useLinearFiltering);
}

_Use_decl_annotations_
Block3DRenderer::Block3DRenderer(ID3D11Device* device, ID3D11ShaderResourceView* walls, ID3D11ShaderResourceView* sprites, bool useLinearFiltering) :
    _device(device), _wallsSRV(walls), _spritesSRV(sprites), _numStaticVertices(0), _numVertices(0),
    _preservePipelineState(false), _ceilingColor(0.3f, 0.3f, 0.3f, 1.0f), _floorColor(0.6f, 0.6f, 0.6f, 1.0f)
{
    assert(device);
    assert(walls);
    assert(sprites);

    device->GetImmediateContext(&_context);

    ComPtr<ID3D11Resource> resource;
    walls->GetResource(&resource);

    ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = resource.As(&texture);
    if (FAILED(hr)) throw std::exception();

    D3D11_TEXTURE2D_DESC td = {};
    texture->GetDesc(&td);
    _wallsSizeInBlocks = XMFLOAT2(td.Width / 64.0f, td.Height / 64.0f);

    sprites->GetResource(&resource);
    hr = resource.As(&texture);
    if (FAILED(hr)) throw std::exception();

    texture->GetDesc(&td);
    _spritesSizeInBlocks = XMFLOAT2(td.Width / 64.0f, td.Height / 64.0f);

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(ClearConstants);
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.StructureByteStride = bd.ByteWidth;
    bd.Usage = D3D11_USAGE_DYNAMIC;

    hr = _device->CreateBuffer(&bd, nullptr, &_clearConstantBuffer);
    if (FAILED(hr)) throw std::exception();

    bd.ByteWidth = sizeof(Constants);
    bd.StructureByteStride = bd.ByteWidth;

    hr = _device->CreateBuffer(&bd, nullptr, &_constantBuffer);
    if (FAILED(hr)) throw std::exception();

    D3D11_INPUT_ELEMENT_DESC clearElems[1] = {};
    clearElems[0].Format = DXGI_FORMAT_R32G32_FLOAT;
    clearElems[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    clearElems[0].SemanticName = "POSITION";

    hr = _device->CreateVertexShader(g_ClearVS, _countof(g_ClearVS), nullptr, &_clearVertexShader);
    if (FAILED(hr)) throw std::exception();

    hr = _device->CreateInputLayout(clearElems, _countof(clearElems), g_ClearVS, _countof(g_ClearVS), &_clearInputLayout);
    if (FAILED(hr)) throw std::exception();

    hr = _device->CreatePixelShader(g_ClearPS, _countof(g_ClearPS), nullptr, &_clearPixelShader);
    if (FAILED(hr)) throw std::exception();

    D3D11_INPUT_ELEMENT_DESC elems[2] = {};
    elems[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    elems[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elems[0].SemanticName = "POSITION";
    elems[1].AlignedByteOffset = sizeof(XMFLOAT3);
    elems[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    elems[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elems[1].SemanticName = "TEXCOORD";

    hr = _device->CreateVertexShader(g_VertexShader, _countof(g_VertexShader), nullptr, &_vertexShader);
    if (FAILED(hr)) throw std::exception();

    hr = _device->CreateInputLayout(elems, _countof(elems), g_VertexShader, _countof(g_VertexShader), &_inputLayout);
    if (FAILED(hr)) throw std::exception();

    hr = _device->CreatePixelShader(g_PixelShader, _countof(g_PixelShader), nullptr, &_pixelShader);
    if (FAILED(hr)) throw std::exception();

    D3D11_SAMPLER_DESC sd = {};
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.Filter = useLinearFiltering ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
    sd.MaxLOD = FLT_MAX;

    hr = _device->CreateSamplerState(&sd, &_pointSampler);
    if (FAILED(hr)) throw std::exception();

    bd.CPUAccessFlags = 0;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(Vertex2D) * 6;
    bd.StructureByteStride = sizeof(Vertex2D);
    bd.Usage = D3D11_USAGE_DEFAULT;

    Vertex2D quad[] =
    {
        { XMFLOAT2(-1, 1) },
        { XMFLOAT2(1, 1) },
        { XMFLOAT2(1, -1) },
        { XMFLOAT2(-1, 1) },
        { XMFLOAT2(1, -1) },
        { XMFLOAT2(-1, -1) },
    };

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = quad;
    init.SysMemPitch = sizeof(quad);
    init.SysMemSlicePitch = init.SysMemPitch;

    hr = _device->CreateBuffer(&bd, &init, &_clearVB);
    if (FAILED(hr)) throw std::exception();

    bd.ByteWidth = sizeof(Vertex) * 6 * MaxQuads;
    bd.StructureByteStride = sizeof(Vertex);

    hr = _device->CreateBuffer(&bd, nullptr, &_vertexBuffer);
    if (FAILED(hr)) throw std::exception();

    _vertices.reset(new Vertex[6 * MaxQuads]);
}

void Block3DRenderer::SetLevelColors(const XMFLOAT3& ceiling, const XMFLOAT3& floor)
{
    _ceilingColor = XMFLOAT4(ceiling.x, ceiling.y, ceiling.z, 1.0f);
    _floorColor = XMFLOAT4(floor.x, floor.y, floor.z, 1.0f);
}

_Use_decl_annotations_
void Block3DRenderer::SetWallLayout(const int16_t* data)
{
    _numVertices = 0;

    const int16_t* p = data;
    for (int32_t y = 0; y < 64; ++y)
    {
        for (int32_t x = 0; x < 64; ++x, ++p)
        {
            if (*p >= 0)
            {
                // Determine image to show on each face
                uint16_t baseIndex = *p;
                uint16_t frontIndex = baseIndex;
                uint16_t backIndex = baseIndex;
                uint16_t leftIndex = baseIndex;
                uint16_t rightIndex = baseIndex;

                // Are any of the surrounding sides a door? If so, use appropriate door jamb image
                if (y > 0 && data[(y - 1) * 64 + x] < -1)
                {
                    backIndex = -(data[(y - 1) * 64 + x] + 2);
                }
                if (y < 63 && data[(y + 1) * 64 + x] < -1)
                {
                    frontIndex = -(data[(y + 1) * 64 + x] + 2);
                }
                if (x > 0 && data[y * 64 + (x - 1)] < -1)
                {
                    leftIndex = -(data[y * 64 + (x - 1)] + 2);
                }
                if (x < 63 && data[y * 64 + (x + 1)] < -1)
                {
                    rightIndex = -(data[y * 64 + (x + 1)] + 2);
                }

                // Darken left/right sides
                ++leftIndex;
                ++rightIndex;

                float cx = x * 64.0f;
                float cx2 = cx + 64.0f;
                float cy = y * 64.0f;
                float cy2 = cy + 64.0f;

                // Front
                AddQuad(XMFLOAT3(cx,    64.0f,  cy2),
                        XMFLOAT3(cx2,   64.0f,  cy2),
                        XMFLOAT3(cx2,   0.0f,   cy2),
                        XMFLOAT3(cx,    0.0f,   cy2),
                        frontIndex, 0, false);

                // Back
                AddQuad(XMFLOAT3(cx2,   64.0f,  cy),
                        XMFLOAT3(cx,    64.0f,  cy),
                        XMFLOAT3(cx,    0.0f,   cy),
                        XMFLOAT3(cx2,   0.0f,   cy),
                        backIndex, 0, false);

                // Left
                AddQuad(XMFLOAT3(cx,    64.0f,  cy),
                        XMFLOAT3(cx,    64.0f,  cy2),
                        XMFLOAT3(cx,    0.0f,   cy2),
                        XMFLOAT3(cx,    0.0f,   cy),
                        leftIndex, 0, false);

                // Right
                AddQuad(XMFLOAT3(cx2,   64.0f,  cy2),
                        XMFLOAT3(cx2,   64.0f,  cy),
                        XMFLOAT3(cx2,   0.0f,   cy),
                        XMFLOAT3(cx2,   0.0f,   cy2),
                        rightIndex, 0, false);

                // Top
                AddQuad(XMFLOAT3(cx,    64.0f,  cy),
                        XMFLOAT3(cx2,   64.0f,  cy),
                        XMFLOAT3(cx2,   64.0f,  cy2),
                        XMFLOAT3(cx,    64.0f,  cy2),
                        baseIndex, 0, false);
            }
        }
    }

    _numStaticVertices = _numVertices;

    D3D11_BOX box = {};
    box.right = sizeof(Vertex) * _numVertices;
    box.bottom = 1;
    box.back = 1;
    _context->UpdateSubresource(_vertexBuffer.Get(), 0, &box, _vertices.get(), box.right, 0);
}

void Block3DRenderer::Begin(const D3D11_VIEWPORT& viewport, const XMFLOAT4X4& view, const XMFLOAT4X4& projection, bool preservePipelineState)
{
    _viewport = viewport;
    _preservePipelineState = preservePipelineState;

    XMVECTOR det;
    XMMATRIX cameraWorld = XMMatrixInverse(&det, XMLoadFloat4x4(&view));
    XMFLOAT3 dir;
    XMStoreFloat3(&dir, XMVectorScale(cameraWorld.r[0], 32.0f));

    _billboardCorners[0] = XMFLOAT2(-dir.x, -dir.z);
    _billboardCorners[1] = XMFLOAT2(dir.x, dir.z);

    if (preservePipelineState)
    {
        // Save pipeline state
        _context->IAGetInputLayout(&_prevInputLayout);
        _context->IAGetPrimitiveTopology(&_prevTopology);
        _context->IAGetVertexBuffers(0, 1, &_prevVB, &_prevStride, &_prevVBOffset);
        _context->VSGetConstantBuffers(0, 1, &_prevCB);
        _context->VSGetShader(&_prevVertexShader, &_prevVSClass, &_prevVSInstances);
        _context->PSGetConstantBuffers(0, 1, &_prevPSCB);
        _context->PSGetShader(&_prevPixelShader, &_prevPSClass, &_prevPSInstances);
        _context->PSGetShaderResources(0, 1, &_prevSRV);
        _context->PSGetSamplers(0, 1, &_prevSampler);
        _context->RSGetViewports(&_prevNumViewports, _prevViewports);
        _context->RSGetState(&_prevRS);
        _context->OMGetDepthStencilState(&_prevDepthStencilState, &_prevStencilRef);
    }

    //
    // Update constant buffers
    //
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    HRESULT hr = _context->Map(_clearConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    if (FAILED(hr)) throw std::exception();

    ClearConstants* clearConstants = (ClearConstants*)mapped.pData;
    clearConstants->CeilingColor = _ceilingColor;
    clearConstants->FloorColor = _floorColor;

    _context->Unmap(_clearConstantBuffer.Get(), 0);

    hr = _context->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    if (FAILED(hr)) throw std::exception();

    Constants* constants = (Constants*)mapped.pData;
    constants->View = view;
    constants->Projection = projection;
    constants->Texture1SizeInBlocks = _wallsSizeInBlocks;
    constants->Texture2SizeInBlocks = _spritesSizeInBlocks;

    _context->Unmap(_constantBuffer.Get(), 0);

    //
    // Common state
    //
    _context->RSSetViewports(1, &_viewport);
    _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _context->RSSetState(nullptr);
    _context->OMSetDepthStencilState(nullptr, 0);

    //
    // Clear the screen with ceiling / floor colors
    //
    _context->IASetInputLayout(_clearInputLayout.Get());
    static const uint32_t Vertex2DStride = sizeof(Vertex2D);
    _context->IASetVertexBuffers(0, 1, _clearVB.GetAddressOf(), &Vertex2DStride, &VertexOffset);
    _context->PSSetConstantBuffers(0, 1, _clearConstantBuffer.GetAddressOf());
    _context->VSSetShader(_clearVertexShader.Get(), nullptr, 0);
    _context->PSSetShader(_clearPixelShader.Get(), nullptr, 0);
    _context->Draw(6, 0);

    //
    // Prep for 3D rendering
    //
    _context->IASetInputLayout(_inputLayout.Get());
    _context->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &VertexStride, &VertexOffset);
    _context->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);

    ID3D11ShaderResourceView* srvs[] = { _wallsSRV.Get(), _spritesSRV.Get() };
    _context->PSSetShaderResources(0, _countof(srvs), srvs);
    _context->PSSetSamplers(0, 1, _pointSampler.GetAddressOf());
}

void Block3DRenderer::DrawSprite(const XMFLOAT2& position, uint16_t spriteIndex)
{
    AddQuad(XMFLOAT3(position.x + _billboardCorners[0].x, 64.0f, position.y + _billboardCorners[0].y),
            XMFLOAT3(position.x + _billboardCorners[1].x, 64.0f, position.y + _billboardCorners[1].y),
            XMFLOAT3(position.x + _billboardCorners[1].x,  0.0f, position.y + _billboardCorners[1].y),
            XMFLOAT3(position.x + _billboardCorners[0].x,  0.0f, position.y + _billboardCorners[0].y),
            spriteIndex, 1, false);
}

void Block3DRenderer::DrawCube(const XMFLOAT2& position, uint16_t wallIndex)
{
    // Front
    AddQuad(XMFLOAT3(position.x - 32.0f, 64.0f, position.y + 32.0f),
            XMFLOAT3(position.x + 32.0f, 64.0f, position.y + 32.0f),
            XMFLOAT3(position.x + 32.0f, 0.0f, position.y + 32.0f),
            XMFLOAT3(position.x - 32.0f, 0.0f, position.y + 32.0f),
            wallIndex, 0, false);

    // Back
    AddQuad(XMFLOAT3(position.x + 32.0f, 64.0f, position.y - 32.0f),
            XMFLOAT3(position.x - 32.0f, 64.0f, position.y - 32.0f),
            XMFLOAT3(position.x - 32.0f, 0.0f, position.y - 32.0f),
            XMFLOAT3(position.x + 32.0f, 0.0f, position.y - 32.0f),
            wallIndex, 0, false);

    // Left
    AddQuad(XMFLOAT3(position.x - 32.0f, 64.0f, position.y - 32.0f),
            XMFLOAT3(position.x - 32.0f, 64.0f, position.y + 32.0f),
            XMFLOAT3(position.x - 32.0f, 0.0f, position.y + 32.0f),
            XMFLOAT3(position.x - 32.0f, 0.0f, position.y - 32.0f),
            wallIndex + 1, 0, false);

    // Right
    AddQuad(XMFLOAT3(position.x + 32.0f, 64.0f, position.y + 32.0f),
            XMFLOAT3(position.x + 32.0f, 64.0f, position.y - 32.0f),
            XMFLOAT3(position.x + 32.0f, 0.0f, position.y - 32.0f),
            XMFLOAT3(position.x + 32.0f, 0.0f, position.y + 32.0f),
            wallIndex + 1, 0, false);

    // Top
    AddQuad(XMFLOAT3(position.x - 32.0f, 64.0f, position.y - 32.0f),
            XMFLOAT3(position.x + 32.0f, 64.0f, position.y - 32.0f),
            XMFLOAT3(position.x + 32.0f, 64.0f, position.y + 32.0f),
            XMFLOAT3(position.x - 32.0f, 64.0f, position.y + 32.0f),
            wallIndex, 0, false);
}

// Draw a dynamic door
void Block3DRenderer::DrawDoor(const XMFLOAT2& position, bool horizontal, uint16_t imageIndex)
{
    if (horizontal)
    {
        AddQuad(XMFLOAT3(position.x - 32.0f, 64.0f, position.y),
                XMFLOAT3(position.x + 32.0f, 64.0f, position.y),
                XMFLOAT3(position.x + 32.0f, 0.0f, position.y),
                XMFLOAT3(position.x - 32.0f, 0.0f, position.y),
                imageIndex, 0, false);

        AddQuad(XMFLOAT3(position.x + 32.0f, 64.0f, position.y),
                XMFLOAT3(position.x - 32.0f, 64.0f, position.y),
                XMFLOAT3(position.x - 32.0f, 0.0f, position.y),
                XMFLOAT3(position.x + 32.0f, 0.0f, position.y),
                imageIndex, 0, true);
    }
    else
    {
        AddQuad(XMFLOAT3(position.x, 64.0f, position.y - 32.0f),
                XMFLOAT3(position.x, 64.0f, position.y + 32.0f),
                XMFLOAT3(position.x, 0.0f,  position.y + 32.0f),
                XMFLOAT3(position.x, 0.0f,  position.y - 32.0f),
                imageIndex, 0, false);

        AddQuad(XMFLOAT3(position.x, 64.0f, position.y + 32.0f),
                XMFLOAT3(position.x, 64.0f, position.y - 32.0f),
                XMFLOAT3(position.x, 0.0f,  position.y - 32.0f),
                XMFLOAT3(position.x, 0.0f,  position.y + 32.0f),
                imageIndex, 0, true);
    }
}

void Block3DRenderer::End()
{
    if (_numVertices > _numStaticVertices)
    {
        D3D11_BOX box = {};
        box.left = sizeof(Vertex) * _numStaticVertices;
        box.right = sizeof(Vertex) * _numVertices;
        box.bottom = 1;
        box.back = 1;
        _context->UpdateSubresource(_vertexBuffer.Get(), 0, &box, _vertices.get() + _numStaticVertices, box.right - box.left, 0);
    }

    _context->Draw(_numVertices, 0);

    _numVertices = _numStaticVertices;

    if (_preservePipelineState)
    {
        // Restore pipeline state
        _context->IASetInputLayout(_prevInputLayout.Get());
        _context->IASetPrimitiveTopology(_prevTopology);
        _context->IASetVertexBuffers(0, 1, _prevVB.GetAddressOf(), &_prevStride, &_prevVBOffset);
        _context->VSSetConstantBuffers(0, 1, &_prevCB);
        _context->VSSetShader(_prevVertexShader.Get(), _prevVSClass.GetAddressOf(), _prevVSInstances);
        _context->PSSetConstantBuffers(0, 1, &_prevPSCB);
        _context->PSSetShader(_prevPixelShader.Get(), _prevPSClass.GetAddressOf(), _prevPSInstances);
        _context->PSSetShaderResources(0, 1, _prevSRV.GetAddressOf());
        _context->PSSetSamplers(0, 1, _prevSampler.GetAddressOf());
        _context->RSSetViewports(_prevNumViewports, _prevViewports);
        _context->RSSetState(_prevRS.Get());
        _context->OMSetDepthStencilState(_prevDepthStencilState.Get(), _prevStencilRef);
    }
}

void Block3DRenderer::AddQuad(const XMFLOAT3& a, const XMFLOAT3& b, const XMFLOAT3& c, const XMFLOAT3& d, uint16_t imageIndex, uint32_t textureIndex, bool flipTextureHorizontally)
{
    if (_numVertices + 6 > 6 * MaxQuads)
    {
        assert(false);
        return;
    }

    uint16_t numBlocksWidth = (textureIndex == 0) ? (uint16_t)_wallsSizeInBlocks.x : (uint16_t)_spritesSizeInBlocks.x;
    float u = (float)(imageIndex % numBlocksWidth);
    float v = (float)(imageIndex / numBlocksWidth);

    if (flipTextureHorizontally)
    {
        _vertices[_numVertices++] = Vertex(a, XMFLOAT2(u + 1, v), textureIndex);
        _vertices[_numVertices++] = Vertex(b, XMFLOAT2(u, v), textureIndex);
        _vertices[_numVertices++] = Vertex(c, XMFLOAT2(u, v + 1), textureIndex);
        _vertices[_numVertices++] = Vertex(a, XMFLOAT2(u + 1, v), textureIndex);
        _vertices[_numVertices++] = Vertex(c, XMFLOAT2(u, v + 1), textureIndex);
        _vertices[_numVertices++] = Vertex(d, XMFLOAT2(u + 1, v + 1), textureIndex);
    }
    else
    {
        _vertices[_numVertices++] = Vertex(a, XMFLOAT2(u, v), textureIndex);
        _vertices[_numVertices++] = Vertex(b, XMFLOAT2(u + 1, v), textureIndex);
        _vertices[_numVertices++] = Vertex(c, XMFLOAT2(u + 1, v + 1), textureIndex);
        _vertices[_numVertices++] = Vertex(a, XMFLOAT2(u, v), textureIndex);
        _vertices[_numVertices++] = Vertex(c, XMFLOAT2(u + 1, v + 1), textureIndex);
        _vertices[_numVertices++] = Vertex(d, XMFLOAT2(u, v + 1), textureIndex);
    }
}
