#include "precomp.h"
#include "primitives.h"
#include "renderer.h"
#include "camera.h"
#include "lightpatch.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

//==============================================================================

struct SceneBuilder
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    void InsertTriangle(const Vertex& a, const Vertex& b, const Vertex& c)
    {
        uint32_t numVertices = static_cast<uint32_t>(vertices.size());

        vertices.push_back(a);
        vertices.push_back(b);
        vertices.push_back(c);

        indices.push_back(numVertices);     // a
        indices.push_back(numVertices + 1); // b
        indices.push_back(numVertices + 2); // c
    }

    void InsertQuad(const Quad& quad)
    {
        uint32_t numVertices = static_cast<uint32_t>(vertices.size());

        vertices.push_back(quad.A);
        vertices.push_back(quad.B);
        vertices.push_back(quad.C);
        vertices.push_back(quad.D);

        indices.push_back(numVertices);     // A
        indices.push_back(numVertices + 1); // B
        indices.push_back(numVertices + 2); // C
        indices.push_back(numVertices);     // A
        indices.push_back(numVertices + 2); // C
        indices.push_back(numVertices + 3); // D
    }
};

//==============================================================================

_Use_decl_annotations_
void Renderer::LoadScene()
{
    // Build test scene

    SceneBuilder builder;

    Quad walls[] =
    {
        // forward
        Quad(
            Vertex(-1.0f, -1.0f, 1.0f),
            Vertex(-1.0f, 1.0f, 1.0f),
            Vertex(1.0f, 1.0f, 1.0f),
            Vertex(1.0f, -1.0f, 1.0f)
        ),
        // backward
        Quad(
            Vertex(1.0f, -1.0f, -1.0f),
            Vertex(1.0f, 1.0f, -1.0f),
            Vertex(-1.0f, 1.0f, -1.0f),
            Vertex(-1.0f, -1.0f, -1.0f)
        ),
        // left
        Quad(
            Vertex(-1.0f, -1.0f, -1.0f),
            Vertex(-1.0f, 1.0f, -1.0f),
            Vertex(-1.0f, 1.0f, 1.0f),
            Vertex(-1.0f, -1.0f, 1.0f)
        ),
        // right
        Quad(
            Vertex(1.0f, -1.0f, 1.0f),
            Vertex(1.0f, 1.0f, 1.0f),
            Vertex(1.0f, 1.0f, -1.0f),
            Vertex(1.0f, -1.0f, -1.0f)
        ),
        // top
        Quad(
            Vertex(-1.0f, 1.0f, 1.0f),
            Vertex(-1.0f, 1.0f, -1.0f),
            Vertex(1.0f, 1.0f, -1.0f),
            Vertex(1.0f, 1.0f, 1.0f)
        ),
        // bottom
        Quad(
            Vertex(-1.0f, -1.0f, -1.0f),
            Vertex(-1.0f, -1.0f, 1.0f),
            Vertex(1.0f, -1.0f, 1.0f),
            Vertex(1.0f, -1.0f, -1.0f)
        ),
        // light panel
        Quad(
            Vertex(-0.9f, -0.875f, -0.125f),
            Vertex(-0.9f, -0.75f, -0.125f),
            Vertex(-0.9f, -0.75f, 0.125f),
            Vertex(-0.9f, -0.875f, 0.125f)
        ),
        // light panel
        Quad(
            Vertex(-0.125f, 0.9f, 0.125f),
            Vertex(-0.125f, 0.9f, -0.125f),
            Vertex(0.125f, 0.9f, -0.125f),
            Vertex(0.125f, 0.9f, 0.125f)
        ),
    };

    Quad cube[] =
    {
        // front
        Quad(
            Vertex(-1.0f, -1.0f, -1.0f),
            Vertex(-1.0f, 1.0f, -1.0f),
            Vertex(1.0f, 1.0f, -1.0f),
            Vertex(1.0f, -1.0f, -1.0f)
        ),
        // back
        Quad(
            Vertex(1.0f, -1.0f, 1.0f),
            Vertex(1.0f, 1.0f, 1.0f),
            Vertex(-1.0f, 1.0f, 1.0f),
            Vertex(-1.0f, -1.0f, 1.0f)
        ),
        // right
        Quad(
            Vertex(1.0f, -1.0f, -1.0f),
            Vertex(1.0f, 1.0f, -1.0f),
            Vertex(1.0f, 1.0f, 1.0f),
            Vertex(1.0f, -1.0f, 1.0f)
        ),
        // left
        Quad(
            Vertex(-1.0f, -1.0f, 1.0f),
            Vertex(-1.0f, 1.0f, 1.0f),
            Vertex(-1.0f, 1.0f, -1.0f),
            Vertex(-1.0f, -1.0f, -1.0f)
        ),
        // bottom
        Quad(
            Vertex(-1.0f, -1.0f, 1.0f),
            Vertex(-1.0f, -1.0f, -1.0f),
            Vertex(1.0f, -1.0f, -1.0f),
            Vertex(1.0f, -1.0f, 1.0f)
        ),
        // top
        Quad(
            Vertex(-1.0f, 1.0f, -1.0f),
            Vertex(-1.0f, 1.0f, 1.0f),
            Vertex(1.0f, 1.0f, 1.0f),
            Vertex(1.0f, 1.0f, -1.0f)
        ),
    };


    XMStoreFloat4x4(&_wallsWorld, XMMatrixScaling(20.0f, 10.0f, 30.0f));

    XMStoreFloat4x4(&_cubeWorld, 
        XMMatrixMultiply(
            XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(45.0f)), XMMatrixScaling(3.0f, 3.0f, 3.0f)),
                XMMatrixTranslation(0, -5.0f, 12.0f)));

    _patchManager->BeginPatchMap(8, 64);

    // walls
    _patchManager->InsertQuad(walls[0], _wallsWorld, XMFLOAT3(0, 0, 0));
    _patchManager->InsertQuad(walls[1], _wallsWorld, XMFLOAT3(0, 0, 0));
    _patchManager->InsertQuad(walls[2], _wallsWorld, XMFLOAT3(0, 0, 0));
    _patchManager->InsertQuad(walls[3], _wallsWorld, XMFLOAT3(0, 0, 0));
    _patchManager->InsertQuad(walls[4], _wallsWorld, XMFLOAT3(0, 0, 0));
    _patchManager->InsertQuad(walls[5], _wallsWorld, XMFLOAT3(0, 0, 0));
    _patchManager->InsertQuad(walls[6], _wallsWorld, XMFLOAT3(0, 0, 0));
    _patchManager->InsertQuad(walls[7], _wallsWorld, XMFLOAT3(1, 0, 0));

    // cube
    _patchManager->InsertQuad(cube[0], _cubeWorld, XMFLOAT3(0, 0, 0));
    _patchManager->InsertQuad(cube[1], _cubeWorld, XMFLOAT3(0, 0, 0));
    _patchManager->InsertQuad(cube[2], _cubeWorld, XMFLOAT3(0, 0, 0));
    _patchManager->InsertQuad(cube[3], _cubeWorld, XMFLOAT3(0, 0, 0));
    _patchManager->InsertQuad(cube[4], _cubeWorld, XMFLOAT3(0, 0, 0));
    _patchManager->InsertQuad(cube[5], _cubeWorld, XMFLOAT3(0, 0, 0));

    _patchManager->EndPatchMap();

    auto& vertices = builder.vertices;
    auto& indices = builder.indices;

    builder.InsertQuad(walls[0]);
    builder.InsertQuad(walls[1]);
    builder.InsertQuad(walls[2]);
    builder.InsertQuad(walls[3]);
    builder.InsertQuad(walls[4]);
    builder.InsertQuad(walls[5]);
    builder.InsertQuad(walls[6]);
    builder.InsertQuad(walls[7]);

    _wallsIndexOffset = 0;
    _wallsIndexCount = static_cast<uint32_t>(indices.size());

    builder.InsertQuad(cube[0]);
    builder.InsertQuad(cube[1]);
    builder.InsertQuad(cube[2]);
    builder.InsertQuad(cube[3]);
    builder.InsertQuad(cube[4]);
    builder.InsertQuad(cube[5]);

    _cubeIndexOffset = _wallsIndexCount;
    _cubeIndexCount = static_cast<uint32_t>(indices.size()) - _cubeIndexOffset;

    uint32_t numVertices = static_cast<uint32_t>(vertices.size());
    uint32_t numIndices = static_cast<uint32_t>(indices.size());

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.StructureByteStride = sizeof(Vertex);
    bd.ByteWidth = numVertices * bd.StructureByteStride;
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = vertices.data();

    HRESULT hr = _device->CreateBuffer(&bd, &init, &_vertices);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.StructureByteStride = sizeof(uint32_t);
    bd.ByteWidth = numIndices * bd.StructureByteStride;

    init.pSysMem = indices.data();

    hr = _device->CreateBuffer(&bd, &init, &_indices);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    const uint32_t stride = sizeof(Vertex);
    const uint32_t offset = 0;

    _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _context->IASetInputLayout(_inputLayout.Get());
    _context->IASetVertexBuffers(0, 1, _vertices.GetAddressOf(), &stride, &offset);
    _context->IASetIndexBuffer(_indices.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void Renderer::BindRenderPipeline()
{
    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->VSSetConstantBuffers(0, 1, _vertexShaderConstantBuffer.GetAddressOf());

    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);
    _context->PSSetShaderResources(0, 1, _lightMapSRV.GetAddressOf());
    _context->PSSetShaderResources(1, 1, _diffuseSRV.GetAddressOf());
    _context->PSSetSamplers(0, 1, _pointSampler.GetAddressOf());

    D3D11_VIEWPORT vp = {};
    vp.Width = 1280.0f;
    vp.Height = 720.0f;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

    _context->RSSetViewports(1, &vp);

}

void Renderer::DrawScene()
{
    // Render
    _vertexShaderConstants.World = _wallsWorld;
    _context->UpdateSubresource(_vertexShaderConstantBuffer.Get(), 0, nullptr, &_vertexShaderConstants, sizeof(_vertexShaderConstants), 0);
    _context->DrawIndexed(_wallsIndexCount, _wallsIndexOffset, 0);

    _vertexShaderConstants.World = _cubeWorld;
    _context->UpdateSubresource(_vertexShaderConstantBuffer.Get(), 0, nullptr, &_vertexShaderConstants, sizeof(_vertexShaderConstants), 0);
    _context->DrawIndexed(_cubeIndexCount, _cubeIndexOffset, 0);
}

//==============================================================================
