#include "precomp.h"
#include "materialpriv.h"

void Material::ApplyMaterial()
{
    static const float blendFactors[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    auto lock = GetGraphics().LockContext();

    _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _context->IASetInputLayout(_inputLayout.Get());
    _context->VSSetShader(_vertexShader.Get(), nullptr, 0);
    _context->PSSetShader(_pixelShader.Get(), nullptr, 0);
    _context->RSSetState(_rasterizer.Get());
    _context->OMSetBlendState(_blendState.Get(), blendFactors, 0xFFFFFFFF);
    _context->OMSetDepthStencilState(_depthState.Get(), 0);
}

_Use_decl_annotations_
void Material::SetVertexShader(const char* filename, VertexFormat format)
{
    size_t length = 0;
    auto buffer = ReadFile(filename, &length);

    ComPtr<ID3D11Device> device;
    _context->GetDevice(&device);

    CHECKHR(device->CreateVertexShader(buffer.get(), length, nullptr, &_vertexShader));

    auto elems = GetVertexElements(format);
    CHECKHR(device->CreateInputLayout(elems.data(), static_cast<uint32_t>(elems.size()), buffer.get(), length, &_inputLayout));
}

_Use_decl_annotations_
void Material::SetPixelShader(const char* filename)
{
    size_t length = 0;
    auto buffer = ReadFile(filename, &length);

    ComPtr<ID3D11Device> device;
    _context->GetDevice(&device);

    CHECKHR(device->CreatePixelShader(buffer.get(), length, nullptr, &_pixelShader));
}
