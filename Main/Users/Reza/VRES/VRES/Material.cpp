#include "Precomp.h"
#include "VRES.h"

using namespace VRES;

_Use_decl_annotations_
Material::Material(const char* name, VRES::Renderer* renderer, const ComPtr<ID3D11DeviceContext>& context) :
    _name(name), _renderer(renderer), _context(context)
{
    _context->GetDevice(&_device);

    ZeroMemory(_cbs, sizeof(_cbs));
    ZeroMemory(_samplers, sizeof(_samplers));
}

Material::~Material()
{
    for (uint32_t stage = 0; stage < (uint32_t)ShaderStage::Max; ++stage)
    {
        for (uint32_t i = 0; i < _countof(_cbs[stage]); ++i)
        {
            if (_cbs[stage][i])
            {
                _cbs[stage][i]->Release();
            }
        }

        for (uint32_t i = 0; i < _countof(_samplers[stage]); ++i)
        {
            if (_samplers[stage][i])
            {
                _samplers[stage][i]->Release();
            }
        }
    }
}

_Use_decl_annotations_
void Material::RenderModels(const Model* const* models, uint32_t numModels, const Light* const* lights, uint32_t numLights, const std::shared_ptr<HMD>& hmd)
{
    ApplyPipeline();

    const OutputDescription* outputs = hmd->Outputs();
    for (uint32_t i = 0; i < hmd->NumOutputs(); ++i)
    {
        _context->RSSetViewports(1, &outputs[i].Viewport);
        RenderModels(outputs[i], models, numModels, lights, numLights);
    }
}

bool Material::CreateVertexShader(VertexFormat vertexFormat, const void* byteCode, size_t byteCodeLength)
{
    const D3D11_INPUT_ELEMENT_DESC* elements;
    uint32_t numElements;
    GetVertexElements(vertexFormat, &elements, &numElements);

    HRESULT hr = _device->CreateInputLayout(elements, numElements, byteCode, byteCodeLength, &_inputLayout);
    if (FAILED(hr)) return false;

    hr = _device->CreateVertexShader(byteCode, byteCodeLength, nullptr, &_vertexShader);
    return SUCCEEDED(hr);
}

bool Material::CreatePixelShader(const void* byteCode, size_t byteCodeLength)
{
    return SUCCEEDED(_device->CreatePixelShader(byteCode, byteCodeLength, nullptr, &_pixelShader));
}

bool Material::CreateCB(ShaderStage stage, uint32_t index, uint32_t size)
{
    assert(index < _countof(_cbs[0]));

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = size;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.StructureByteStride = bd.ByteWidth;
    bd.Usage = D3D11_USAGE_DYNAMIC;

    return SUCCEEDED(_device->CreateBuffer(&bd, nullptr, &_cbs[(uint32_t)stage][index]));
}

void* Material::LockCB(ShaderStage stage, uint32_t index)
{
    assert(index < _countof(_cbs[0]));
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    if (FAILED(_context->Map(_cbs[(uint32_t)stage][index], 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
    {
        return nullptr;
    }
    return mapped.pData;
}

void Material::UnlockCB(ShaderStage stage, uint32_t index)
{
    assert(index < _countof(_cbs[0]));
    _context->Unmap(_cbs[(uint32_t)stage][index], 0);
}

void Material::SetSampler(ShaderStage stage, uint32_t index, const ComPtr<ID3D11SamplerState>& sampler)
{
    assert(index < _countof(_samplers[0]));
    sampler.CopyTo(&_samplers[(uint32_t)stage][index]);
}

void Material::SetBlendState(_In_ const ComPtr<ID3D11BlendState>& blendState)
{
    _blendState = blendState;
}

void Material::SetDepthStencilState(_In_ const ComPtr<ID3D11DepthStencilState>& depthStencilState)
{
    _depthStencilState = depthStencilState;
}

void Material::SetRasterizerState(_In_ const ComPtr<ID3D11RasterizerState>& rasterizerState)
{
    _rasterizerState = rasterizerState;
}

void Material::ApplyPipeline()
{
    _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _context->IASetInputLayout(_inputLayout.Get());

    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->VSSetConstantBuffers(0, _countof(_cbs[0]), _cbs[(uint32_t)ShaderStage::Vertex]);
    _context->VSSetSamplers(0, _countof(_samplers[0]), _samplers[(uint32_t)ShaderStage::Vertex]);

    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);
    _context->PSSetConstantBuffers(0, _countof(_cbs[0]), _cbs[(uint32_t)ShaderStage::Pixel]);
    _context->PSSetSamplers(0, _countof(_samplers[0]), _samplers[(uint32_t)ShaderStage::Pixel]);

    static const float blendFactor[] = { 1, 1, 1, 1 };
    _context->OMSetDepthStencilState(_depthStencilState.Get(), 0);
    _context->OMSetBlendState(_blendState.Get(), blendFactor, UINT_MAX);
    _context->RSSetState(_rasterizerState.Get());
}
