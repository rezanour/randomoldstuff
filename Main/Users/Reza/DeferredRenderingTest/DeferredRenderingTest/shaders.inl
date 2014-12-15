#pragma once

template <typename TConstants>
Microsoft::WRL::ComPtr<ID3D11Buffer> CreateConstantBuffer(ID3D11Device* device)
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(TConstants);
    bd.StructureByteStride = sizeof(TConstants);
    bd.Usage = D3D11_USAGE_DEFAULT;

    ComPtr<ID3D11Buffer> buffer;
    if (FAILED(device->CreateBuffer(&bd, nullptr, &buffer)))
    {
        throw std::exception();
    }

    return buffer;
}

template <typename TConstants>
void UpdateConstantBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const TConstants& constants)
{
    context->UpdateSubresource(buffer, 0, nullptr, &constants, sizeof(constants), 0);
}
