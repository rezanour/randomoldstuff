#pragma once

#include <Geometry.h>

// Shaders
ComPtr<ID3D11VertexShader> LoadVertexShader(_In_ const ComPtr<ID3D11Device>& device, _In_z_ const wchar_t* filename, _Out_ std::unique_ptr<uint8_t[]>& data, _Out_ size_t* size);
ComPtr<ID3D11PixelShader> LoadPixelShader(_In_ const ComPtr<ID3D11Device>& device, _In_z_ const wchar_t* filename);
void GetVertexFormatElements(_In_ VertexFormat format, _Out_ std::vector<D3D11_INPUT_ELEMENT_DESC>& elements);
