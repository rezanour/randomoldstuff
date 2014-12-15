#include <Precomp.h>
#include "Shaders.h"

static std::unique_ptr<uint8_t[]> LoadCompiledShader(_In_z_ const wchar_t* filename, _Out_ size_t* size)
{
    FileHandle file(CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file.IsValid())
    {
        throw std::exception();
    }

    DWORD numBytes = GetFileSize(file.Get(), nullptr);
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[numBytes]);

    DWORD bytesRead = 0;
    if (!ReadFile(file.Get(), buffer.get(), numBytes, &bytesRead, nullptr) || bytesRead != numBytes)
    {
        throw std::exception();
    }

    *size = static_cast<size_t>(numBytes);

    return buffer;
}

_Use_decl_annotations_
ComPtr<ID3D11VertexShader> LoadVertexShader(const ComPtr<ID3D11Device>& device, const wchar_t* filename, std::unique_ptr<uint8_t[]>& data, size_t* size)
{
    data = LoadCompiledShader(filename, size);

    ComPtr<ID3D11VertexShader> shader;
    HRESULT hr = device->CreateVertexShader(data.get(), *size, nullptr, &shader);
    if (FAILED(hr))
    {
        throw std::exception();
    }

    return shader;
}

_Use_decl_annotations_
ComPtr<ID3D11PixelShader> LoadPixelShader(const ComPtr<ID3D11Device>& device, const wchar_t* filename)
{
    size_t size = 0;
    auto buffer = LoadCompiledShader(filename, &size);

    ComPtr<ID3D11PixelShader> shader;
    if (FAILED(device->CreatePixelShader(buffer.get(), size, nullptr, &shader)))
    {
        throw std::exception();
    }

    return shader;
}

_Use_decl_annotations_
void GetVertexFormatElements(VertexFormat format, std::vector<D3D11_INPUT_ELEMENT_DESC>& elements)
{
    elements.clear();

    switch (format)
    {
    case VertexFormat::Position2DProj:
        {
            D3D11_INPUT_ELEMENT_DESC elem = {};
            elem.Format = DXGI_FORMAT_R32G32_FLOAT;
            elem.SemanticName = "POSITION";
            elements.push_back(elem);
        }
        break;

    case VertexFormat::Position2DTexture:
        {
            D3D11_INPUT_ELEMENT_DESC elem = {};
            elem.Format = DXGI_FORMAT_R32G32_FLOAT;
            elem.SemanticName = "POSITION";
            elements.push_back(elem);

            elem.AlignedByteOffset = sizeof(XMFLOAT2);
            elem.Format = DXGI_FORMAT_R32G32_FLOAT;
            elem.SemanticName = "TEXCOORD";
            elements.push_back(elem);
        }
        break;

    case VertexFormat::Position3D:
        {
            D3D11_INPUT_ELEMENT_DESC elem = {};
            elem.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            elem.SemanticName = "POSITION";
            elements.push_back(elem);
        }
        break;

    case VertexFormat::Position3DColor:
        {
            D3D11_INPUT_ELEMENT_DESC elem = {};
            elem.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            elem.SemanticName = "POSITION";
            elements.push_back(elem);

            elem.AlignedByteOffset = sizeof(XMFLOAT3);
            elem.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            elem.SemanticName = "COLOR";
            elements.push_back(elem);
    }
        break;

    case VertexFormat::PositionNormalTangentTexture:
        {
            D3D11_INPUT_ELEMENT_DESC elem = {};
            elem.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            elem.SemanticName = "POSITION";
            elements.push_back(elem);

            elem.AlignedByteOffset = sizeof(XMFLOAT3);
            elem.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            elem.SemanticName = "NORMAL";
            elements.push_back(elem);
    
            elem.AlignedByteOffset = 2 * sizeof(XMFLOAT3);
            elem.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            elem.SemanticName = "TANGENT";
            elements.push_back(elem);

            elem.AlignedByteOffset = 3 * sizeof(XMFLOAT3);
            elem.Format = DXGI_FORMAT_R32G32_FLOAT;
            elem.SemanticName = "TEXCOORD";
            elements.push_back(elem);
        }
        break;

    default:
        throw std::exception();
    }
}
