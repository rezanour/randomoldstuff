#include <Precomp.h>
#include "GraphicsSystemImpl.h"
#include "GraphicsSceneImpl.h"
#include "GeometryImpl.h"
#include "TextureImpl.h"
#include "RenderPass.h"

_Use_decl_annotations_
RenderPass::RenderPass(const std::shared_ptr<GraphicsSystem>& graphics, const ComPtr<ID3D11DeviceContext>& context) :
    _graphics(graphics), _context(context)
{
    _context->GetDevice(&_device);

    Position2DProjVertex vertices[] =
    {
        { XMFLOAT2(-1, -1) },
        { XMFLOAT2(-1, 1) },
        { XMFLOAT2(1, 1) },
        { XMFLOAT2(1, -1) },
    };

    uint32_t indices[] =
    {
        0, 1, 2, 0, 2, 3,
    };

    _quad = Geometry::Create(_device, 4, VertexFormat::Position2DProj, vertices, 6, indices);
}

RenderPass::~RenderPass()
{
}

_Use_decl_annotations_
void RenderPass::DrawScene(const std::shared_ptr<IGraphicsScene>& scene, const XMFLOAT4X4& view, const XMFLOAT4X4& projection)
{
    BeginPass();

    // Input Assembler
    _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _context->IASetInputLayout(_inputLayout.Get());

    HandleDrawScene(scene, view, projection);

    EndPass();
}

const ComPtr<ID3D11Device>& RenderPass::GetDevice() const
{
    return _device;
}

const ComPtr<ID3D11DeviceContext>& RenderPass::GetContext() const
{
    return _context;
}

_Use_decl_annotations_
void RenderPass::LoadShaders(const wchar_t* vertexShader, VertexFormat vertexFormat, const wchar_t* pixelShader)
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> elems;
    GetVertexFormatElements(vertexFormat, elems);

    LoadShaders(vertexShader, elems, pixelShader);
}

_Use_decl_annotations_
void RenderPass::LoadShaders(const wchar_t* vertexShader, const std::vector<D3D11_INPUT_ELEMENT_DESC>& elems, const wchar_t* pixelShader)
{
    std::unique_ptr<uint8_t[]> data;
    size_t size = 0;
    _vertexShader = LoadVertexShader(_device, vertexShader, data, &size);

    HRESULT hr = _device->CreateInputLayout(elems.data(), static_cast<uint32_t>(elems.size()), data.get(), size, &_inputLayout);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    if (pixelShader != nullptr)
    {
        _pixelShader = LoadPixelShader(_device, pixelShader);
    }
}

_Use_decl_annotations_
void RenderPass::CreateVSConstantBuffer(uint32_t slot, uint32_t size)
{
    assert(slot < _countof(_vsConstantBuffers));

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = size;
    bd.StructureByteStride = bd.ByteWidth;
    bd.Usage = D3D11_USAGE_DEFAULT;

    HRESULT hr = _device->CreateBuffer(&bd, nullptr, &_vsConstantBuffers[slot]);
    if (FAILED(hr))
    {
        throw std::exception();
    }
}

_Use_decl_annotations_
void RenderPass::CreatePSConstantBuffer(uint32_t slot, uint32_t size)
{
    assert(slot < _countof(_psConstantBuffers));

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = size;
    bd.StructureByteStride = bd.ByteWidth;
    bd.Usage = D3D11_USAGE_DEFAULT;

    HRESULT hr = _device->CreateBuffer(&bd, nullptr, &_psConstantBuffers[slot]);
    if (FAILED(hr))
    {
        throw std::exception();
    }
}

_Use_decl_annotations_
void RenderPass::UpdateVSConstantBuffer(uint32_t slot, const void* data, uint32_t size)
{
    assert(slot < _countof(_vsConstantBuffers));
    _context->UpdateSubresource(_vsConstantBuffers[slot].Get(), 0, nullptr, data, size, 0);
}

_Use_decl_annotations_
void RenderPass::UpdatePSConstantBuffer(uint32_t slot, const void* data, uint32_t size)
{
    assert(slot < _countof(_psConstantBuffers));
    _context->UpdateSubresource(_psConstantBuffers[slot].Get(), 0, nullptr, data, size, 0);
}

_Use_decl_annotations_
void RenderPass::SetBlendState(const ComPtr<ID3D11BlendState>& blendState)
{
    _blendState = blendState;
}

_Use_decl_annotations_
void RenderPass::SetSampler(uint32_t slot, const ComPtr<ID3D11SamplerState>& sampler)
{
    assert(slot < _countof(_samplers));
    _samplers[slot] = sampler;
}

_Use_decl_annotations_
void RenderPass::SetShaderResource(uint32_t slot, const std::shared_ptr<Texture>& texture)
{
    assert(slot < _countof(_psShaderResources));
    _psShaderResources[slot] = texture;
}

void RenderPass::UpdateShaderResources()
{
    ID3D11ShaderResourceView* shaderResources[_countof(_psShaderResources)] = {};
    for (auto i = 0; i < _countof(_psShaderResources); ++i)
    {
        shaderResources[i] = _psShaderResources[i] != nullptr ? _psShaderResources[i]->GetShaderResourceView().Get() : nullptr;
    }

    _context->PSSetShaderResources(0, _countof(shaderResources), shaderResources);
}

_Use_decl_annotations_
void RenderPass::SetRenderTarget(uint32_t slot, const std::shared_ptr<Texture>& texture)
{
    assert(slot < _countof(_renderTargets));
    _renderTargets[slot] = texture->GetRenderTargetView();
}

_Use_decl_annotations_
void RenderPass::SetRenderTarget(uint32_t slot, const ComPtr<ID3D11RenderTargetView>& rtv)
{
    assert(slot < _countof(_renderTargets));
    _renderTargets[slot] = rtv;
}

_Use_decl_annotations_
void RenderPass::SetDepthBuffer(const std::shared_ptr<Texture>& texture)
{
    _depthBuffer = texture;
}

_Use_decl_annotations_
void RenderPass::SetDepthState(const ComPtr<ID3D11DepthStencilState>& depthState)
{
    _depthState = depthState;
}

void RenderPass::DrawFullScreenQuad()
{
    uint32_t stride = _quad->GetVertexStride();
    uint32_t offset = 0;

    _context->IASetIndexBuffer(_quad->GetIndices().Get(), DXGI_FORMAT_R32_UINT, 0);
    _context->IASetVertexBuffers(0, 1, _quad->GetVertices().GetAddressOf(), &stride, &offset);
    _context->DrawIndexed(_quad->GetNumIndices(), 0, 0);
}

void RenderPass::BeginPass()
{
    ID3D11Buffer* vsConstantBuffers[_countof(_vsConstantBuffers)] = {};
    for (auto i = 0; i < _countof(_vsConstantBuffers); ++i)
    {
        vsConstantBuffers[i] = _vsConstantBuffers[i].Get();
    }

    ID3D11Buffer* psConstantBuffers[_countof(_psConstantBuffers)] = {};
    for (auto i = 0; i < _countof(_psConstantBuffers); ++i)
    {
        psConstantBuffers[i] = _psConstantBuffers[i].Get();
    }

    ID3D11SamplerState* samplers[_countof(_samplers)] = {};
    for (auto i = 0; i < _countof(_samplers); ++i)
    {
        samplers[i] = _samplers[i].Get();
    }

    ID3D11RenderTargetView* renderTargets[_countof(_renderTargets)] = {};
    for (auto i = 0; i < _countof(_renderTargets); ++i)
    {
        renderTargets[i] = _renderTargets[i] != nullptr ? _renderTargets[i].Get() : nullptr;
    }

    // Vertex Shader
    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->VSSetConstantBuffers(0, _countof(vsConstantBuffers), vsConstantBuffers);

    // Pixel Shader. OM step is only valid if there is a pixel shader bound
    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);
    if (_pixelShader != nullptr)
    {
        _context->PSSetConstantBuffers(0, _countof(psConstantBuffers), psConstantBuffers);
        _context->PSSetSamplers(0, _countof(samplers), samplers);
        UpdateShaderResources();

        // Output Merger
        _context->OMSetRenderTargets(_countof(renderTargets), renderTargets, (_depthBuffer != nullptr ? _depthBuffer->GetDepthStencilView().Get() : nullptr));

        static const float blendFactor[] = { 1, 1, 1, 1 };
        _context->OMSetBlendState(_blendState.Get(), blendFactor, 0xFFFFFFFF);
    }

    _context->OMSetDepthStencilState(_depthState.Get(), 0);
}

void RenderPass::EndPass()
{
    ID3D11Buffer* constantBuffers[_countof(_vsConstantBuffers)] = {};
    _context->VSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);
    _context->PSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);

    ID3D11ShaderResourceView* nullShaderResources[_countof(_psShaderResources)] = {};
    _context->PSSetShaderResources(0, _countof(nullShaderResources), nullShaderResources);

    ID3D11RenderTargetView* nullRenderTargets[_countof(_renderTargets)] = {};
    _context->OMSetRenderTargets(_countof(nullRenderTargets), nullRenderTargets, nullptr);

    static const float blendFactor[] = { 1, 1, 1, 1 };
    _context->OMSetBlendState(nullptr, blendFactor, 0xFFFFFFFF);

    _context->OMSetDepthStencilState(nullptr, 0);
}
