#include "StdAfx.h"
#include "Geometry.h"
#include "Renderer.h"
#include <CoreServices\L3DGraphicsSerializers.h>
#include <GDK\Shaders.h>

using namespace Lucid3D;
using namespace GDK;
using namespace L3DMath;

HRESULT Geometry::CreateFullscreenQuad(_In_ Renderer* pRenderer, _In_ IContentManagerPtr& spContentManager, _Inout_ GeometryPtr& spGeometry)
{
    // HACK: We use a 'magic' content id to mean full screen quad
    return Geometry::Create(pRenderer, spContentManager, 0xffffffffffffffff, true, spGeometry);
}

Geometry::Geometry(_In_ Renderer* pRenderer, IContentManagerPtr& spContentManager, _In_ uint64 contentId)
    : ResourceBase(pRenderer, spContentManager, contentId)
{
}

GDK_METHODIMP Geometry::OnLoad(_In_ Renderer* pRenderer, _In_ IContentManagerPtr spContentManager, _In_ uint64 contentId)
{
    HRESULT hr = S_OK;

    ISNOTNULL(spContentManager, E_INVALIDARG);

    // HACK HACK
    if (contentId == 0xffffffffffffffff)
    {
        ULONG cbRead = 0;
        D3D11_BUFFER_DESC bufferDesc = {0};
        D3D11_SUBRESOURCE_DATA subData = {0};

        // QUAD
        Vector2 vertices[] = 
        {
            Vector2(-1, 1),
            Vector2(1, 1),
            Vector2(1, -1),
            Vector2(-1, -1),
        };

        CHECKHR(VertexBuffer::Create(pRenderer, vertices, _countof(vertices), sizeof(vertices[0]), _spVertexBuffer));

        uint32 indices[] =
        {
            0, 1, 2,
            0, 2, 3,
        };

        CHECKHR(IndexBuffer::Create(pRenderer, indices, _countof(indices), sizeof(indices[0]), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, _spIndexBuffer));

        _vertexChannels[ShaderParameter::SemanticEnum::Position] = 0;
    }
    else
    {
        stde::com_ptr<IStream> spStream;
        std::vector<byte> vertices;
        std::vector<uint32> indices;
        size_t vertexStride, indexStride = sizeof(uint32), numVertices;
        D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        ISNOTNULL(spContentManager, E_INVALIDARG);

        CHECKHR(spContentManager->GetStream(contentId, &spStream));

        CHECKHR(L3DGraphics::GeometryData::Load2(spStream, &vertexStride, &numVertices, vertices, indices));

        CHECKHR(VertexBuffer::Create(pRenderer, vertices.data(), numVertices, vertexStride, _spVertexBuffer));
        CHECKHR(IndexBuffer::Create(pRenderer, indices.data(), indices.size(), indexStride, topology, _spIndexBuffer));

        _vertexChannels[ShaderParameter::SemanticEnum::Position] = 0;
        _vertexChannels[ShaderParameter::SemanticEnum::Normal] = 12;
        _vertexChannels[ShaderParameter::SemanticEnum::TexCoord] = 24;
    }

EXIT
    return hr;
}

GDK_METHODIMP Geometry::OnUnload()
{
    // Release resources
    _vertexChannels.clear();
    _inputLayouts.clear();
    _spVertexBuffer = nullptr;
    _spIndexBuffer = nullptr;
    return S_OK;
}

HRESULT Geometry::GenerateInputLayout(_In_ ShaderPtr& spShader)
{
    HRESULT hr = S_OK;

    // see if we already have it
    InputLayoutMap::iterator it = _inputLayouts.find(spShader->GetId());
    if (it != _inputLayouts.end())
        return S_OK;

    D3D11_INPUT_ELEMENT_DESC inputElem = {0};
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;

    // HACK HACK:
    if (GetContentId() == 0xffffffffffffffff)
    {
        inputElem.Format = DXGI_FORMAT_R32G32_FLOAT;
        inputElem.SemanticName = "POSITION";
        inputElements.push_back(inputElem);
    }
    else
    {
        // HACK HACK: should actually compute this off of our inputs & shader
        inputElem.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputElem.SemanticName = "POSITION";
        inputElements.push_back(inputElem);
        inputElem.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputElem.SemanticName = "NORMAL";
        inputElem.AlignedByteOffset = 12;
        inputElements.push_back(inputElem);
        inputElem.Format = DXGI_FORMAT_R32G32_FLOAT;
        inputElem.SemanticName = "TEXCOORD";
        inputElem.AlignedByteOffset = 24;
        inputElements.push_back(inputElem);
    }

    InputLayoutPtr inputLayout;
    stde::com_ptr<ID3DBlob> spCode = spShader->GetShaderCode();

    DevicePtr spDevice;
    _spVertexBuffer->GetBuffer()->GetDevice(&spDevice);
    CHECKHR(spDevice->CreateInputLayout(inputElements.data(), static_cast<UINT>(inputElements.size()), spCode->GetBufferPointer(), spCode->GetBufferSize(), &inputLayout));
    _inputLayouts[spShader->GetId()] = inputLayout;

EXIT
    return hr;
}

HRESULT Geometry::Draw(_In_ uint32 shaderId, _In_ ContextPtr& spContext)
{
    HRESULT hr = S_OK;

    if (!IsLoaded())
    {
        stde::ref_counted_ptr<GDK::IContentTag> spTag;
        GetContentManager()->GetContentTag(GetContentId(), &spTag);
        DebugWarning("Trying to draw unloaded geometry: %s", spTag->GetName());
        return S_OK;
    }

    // Do we have a compatible layout for this shader?
    InputLayoutMap::iterator it = _inputLayouts.find(shaderId);
    ISTRUE(it != _inputLayouts.end(), E_UNEXPECTED);

    ISNOTNULL(spContext, E_INVALIDARG);

    // Setup the input layout
    spContext->IASetInputLayout(it->second);

    ID3D11Buffer* buffers[] = { _spVertexBuffer->GetBuffer() };
    uint32 strides[] = { _spVertexBuffer->GetVertexStride() };
    uint32 offsets[] = { 0 };

    spContext->IASetVertexBuffers(0, 1, buffers, strides, offsets);
    spContext->IASetIndexBuffer(_spIndexBuffer->GetBuffer(), _spIndexBuffer->GetIndexStride() == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
    spContext->IASetPrimitiveTopology(_spIndexBuffer->GetTopology());

    spContext->DrawIndexed(_spIndexBuffer->GetNumIndices(), 0, 0);

EXIT
    return hr;
}

