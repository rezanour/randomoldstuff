#pragma once

// NOTE: templates must be declared inline and defined in the .inl to prevent duplicate template instantiation across files

template <typename TConstants>
inline Microsoft::WRL::ComPtr<ID3D11Buffer> CreateConstantBuffer(ID3D11Device* device);

template <typename TConstants>
inline void UpdateConstantBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const TConstants& constants);

Microsoft::WRL::ComPtr<ID3D11VertexShader> LoadVertexShader(ID3D11Device* device, const wchar_t* filename);

Microsoft::WRL::ComPtr<ID3D11PixelShader> LoadPixelShader(ID3D11Device* device, const wchar_t* filename);

[uuid("D7E29559-D327-4E54-984F-0B6AF82A4CED")]
struct ICachedShaderByteCode : IUnknown
{
    virtual const void* GetData() const = 0;
    virtual size_t GetSize() const = 0;
};

#include "shaders.inl"
