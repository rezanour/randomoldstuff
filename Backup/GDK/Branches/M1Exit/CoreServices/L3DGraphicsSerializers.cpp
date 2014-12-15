#include "L3DGraphicsSerializers.h"
#include "StreamHelper.h"
#include "Debug.h"
#include <stde\types.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#include <GDK\Tools\geometryresource.h>
#include <GDK\Tools\textureresource.h>

using namespace L3DGraphics;
using namespace CoreServices;
using namespace GDK;

static GDK::ShaderParameter ParseParameter(_In_ const D3D11_SIGNATURE_PARAMETER_DESC& parameter);

//
// Geometry Format overview:
//
// 4 bytes - numVertices
// 2 bytes - vertexStride
// 2 bytes - numVertexLayoutElements
// numVertexLayoutElements * 12 bytes each - vertexLayout
// numVertices * vertexStride bytes - vertices
// 2 bytes - topology
// 4 bytes - numIndices
// 1 byte - indexStride
// numIndices * indexStride - indices

HRESULT GeometryData::Save( _In_ stde::com_ptr<IStream> spStream, 
                            _In_ const std::vector<byte>& vertices, _In_ size_t numVertices, _In_ size_t vertexStride, _In_ const std::vector<GDK::ShaderParameter>& vertexLayout, 
                            _In_ const std::vector<uint32>& indices, _In_ size_t indexStride, _In_ D3D11_PRIMITIVE_TOPOLOGY topology)
{
    HRESULT hr = S_OK;

    ISNOTNULL(spStream, E_INVALIDARG);

    {
        ulong cbWritten;

        StreamHelper writer(spStream);

        CHECKHR((writer.WriteValue(numVertices, 4)));
        CHECKHR((writer.WriteValue(vertexStride, 2)));
        CHECKHR((writer.WriteValue(vertexLayout.size(), 2)));

        for (size_t i = 0; i < vertexLayout.size(); i++)
        {
            CHECKHR((writer.WriteValue(vertexLayout[i])));
        }

        CHECKHR(spStream->Write(vertices.data(), static_cast<ulong>(numVertices * vertexStride), &cbWritten));

        CHECKHR((writer.WriteValue(topology, 2)));
        CHECKHR((writer.WriteValue(indices.size(), 4)));
        CHECKHR((writer.WriteValue(indexStride, 1)));

        CHECKHR(spStream->Write(indices.data(), static_cast<ulong>(indices.size() * indexStride), &cbWritten));
    }

EXIT
    return hr;
}

HRESULT GeometryData::Load( _In_ stde::com_ptr<IStream> spStream, 
                            _Out_ std::vector<byte>& vertices, _Out_ size_t& numVertices, _Out_ size_t& vertexStride, _Out_ std::vector<GDK::ShaderParameter>& vertexLayout, 
                            _Out_ std::vector<uint32>& indices, _Out_ size_t& indexStride, _Out_ D3D11_PRIMITIVE_TOPOLOGY& topology)
{
    HRESULT hr = S_OK;

    ISNOTNULL(spStream, E_INVALIDARG);

    {
        size_t numIndices;
        size_t numVertexLayoutElements;
        ulong cbRead, totalVertexBytes, totalIndexBytes;

        StreamHelper reader(spStream);

        CHECKHR((reader.ReadValue(numVertices, 4)));
        CHECKHR((reader.ReadValue(vertexStride, 2)));
        CHECKHR((reader.ReadValue(numVertexLayoutElements, 2)));

        vertexLayout.resize(numVertexLayoutElements);
        for (size_t i = 0; i < numVertexLayoutElements; i++)
        {
            CHECKHR((reader.ReadValue(vertexLayout[i])));
        }

        totalVertexBytes = static_cast<ulong>(numVertices * vertexStride);
        vertices.resize(totalVertexBytes);
        CHECKHR(spStream->Read(vertices.data(), totalVertexBytes, &cbRead));

        CHECKHR((reader.ReadValue(topology, 2)));
        CHECKHR((reader.ReadValue(numIndices, 4)));
        CHECKHR((reader.ReadValue(indexStride, 1)));

        totalIndexBytes = static_cast<ulong>(numIndices * indexStride);
        indices.resize(totalIndexBytes);
        CHECKHR(spStream->Read(indices.data(), totalIndexBytes, &cbRead));
    }

EXIT
    return hr;
}

HRESULT GeometryData::Load2(_In_ stde::com_ptr<IStream> spStream, _Out_ size_t* pStride, _Out_ size_t* pVertexCount, _Out_ std::vector<byte>& vertexData, _Out_ std::vector<uint32>& indices)
{
    HRESULT hr = S_OK;

    GEOMETRY_RESOURCE_FILEHEADER geoHeader;
    std::vector<GEOMETRY_RESOURCE_VERTEX> vertices;
    ULONG cbRead = 0;

    StreamHelper stream(spStream);

    CHECKHR(stream.ReadValue(geoHeader));

    vertices.resize(geoHeader.VertexCount);
    indices.resize(geoHeader.IndicesCount);

    *pVertexCount = geoHeader.VertexCount;

    CHECKHR(spStream->Read(vertices.data(), static_cast<ULONG>(vertices.size() * sizeof(vertices[0])), &cbRead));
    CHECKHR(spStream->Read(indices.data(), static_cast<ULONG>(indices.size() * sizeof(indices[0])), &cbRead));

    size_t stride = sizeof(L3DMath::Vector3) + sizeof(L3DMath::Vector3) + sizeof(L3DMath::Vector2);
    *pStride = stride;
    vertexData.resize(vertices.size() * stride);

    byte* pPositions = vertexData.data();
    byte* pNormals = vertexData.data() + sizeof(L3DMath::Vector3);
    byte* pTexcoords = vertexData.data() + (2 * sizeof(L3DMath::Vector3));

    for (size_t i = 0; i < vertices.size(); i++)
    {
        *reinterpret_cast<L3DMath::Vector3*>(pPositions) = vertices[i].Position;
        *reinterpret_cast<L3DMath::Vector3*>(pNormals) = vertices[i].Normal;
        *reinterpret_cast<L3DMath::Vector2*>(pTexcoords) = vertices[i].TextureCoord0;

        pPositions += stride;
        pNormals += stride;
        pTexcoords += stride;
    }

EXIT
    return hr;
}


HRESULT GDK_API TextureData::Load(_In_ stde::com_ptr<IStream>& spStream,
                                  _In_ stde::com_ptr<ID3D11Device>& spDevice,
                                  _Deref_out_ ID3D11ShaderResourceView** ppView)
{
    HRESULT hr = S_OK;

    TEXTURE_RESOURCE_FILEHEADER header;
    TEXTURE_RESOURCE_IMAGE image;
    D3D11_TEXTURE2D_DESC textureDesc = {0};
    ULONG cbRead = 0;

    StreamHelper stream(spStream);

    CHECKHR(stream.ReadValue(header));

    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.ArraySize = 1;
    textureDesc.Format = static_cast<DXGI_FORMAT>(header.metadata.format);
    textureDesc.Height = header.metadata.height;
    textureDesc.Width = header.metadata.width;
    textureDesc.MipLevels = 1;

    CHECKHR(stream.ReadValue(image));

    {
        std::unique_ptr<byte[]> buffer(new byte[image.cbPixels]);
        D3D11_SUBRESOURCE_DATA subData = {0};
        stde::com_ptr<ID3D11Texture2D> texture;

        CHECKHR(spStream->Read(buffer.get(), image.cbPixels, &cbRead));

        subData.pSysMem = buffer.get();
        subData.SysMemPitch = image.rowPitch;
        subData.SysMemSlicePitch = image.slicePitch;

        CHECKHR(spDevice->CreateTexture2D(&textureDesc, &subData, &texture));

        CHECKHR(spDevice->CreateShaderResourceView(texture, nullptr, ppView));
    }

EXIT
    return hr;
}

HRESULT VertexShaderParser::Parse(  _In_ const std::string& shaderCode, _In_ const std::string& entryPoint, _In_ const std::string& target,
                                    _Out_ std::vector<GDK::ShaderParameter>& signature, 
                                    _Out_ std::vector<GDK::ShaderConstant>& constants)
{
    HRESULT hr = S_OK;

    stde::com_ptr<ID3DBlob> spCode;
    stde::com_ptr<ID3DBlob> spErrors;
    stde::com_ptr<ID3D11ShaderReflection> spReflect;
    D3D11_SHADER_DESC shaderDesc = {0};

    ISFALSE(shaderCode.empty(), E_INVALIDARG);

    uint compilerFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    compilerFlags |= D3DCOMPILE_DEBUG;
#endif

    hr = D3DCompile(shaderCode.c_str(), shaderCode.size(), nullptr, nullptr, nullptr, entryPoint.c_str(), target.c_str(), compilerFlags, 0, &spCode, &spErrors);
    if (FAILED(hr))
    {
        PSTR szErrors = static_cast<PSTR>(spErrors->GetBufferPointer());
        DebugError("D3DCompile failed!\n%s", szErrors);
        CHECKHR(hr);
    }

    CHECKHR(D3DReflect(spCode->GetBufferPointer(), spCode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)(&spReflect)));
    CHECKHR(spReflect->GetDesc(&shaderDesc));

    signature.clear();
    for (uint i = 0; i < shaderDesc.InputParameters; i++)
    {
        D3D11_SIGNATURE_PARAMETER_DESC signatureParameter;
        CHECKHR(spReflect->GetInputParameterDesc(i, &signatureParameter));
        signature.push_back(ParseParameter(signatureParameter));
    }

    constants.clear();
    for (uint i = 0; i < shaderDesc.ConstantBuffers; i++)
    {
    }

EXIT
    if (FAILED(hr))
    {
        signature.clear();
        constants.clear();
    }
    return hr;
}

GDK::ShaderParameter ParseParameter(_In_ const D3D11_SIGNATURE_PARAMETER_DESC& parameter)
{
    GDK::ShaderParameter shaderParam;
    ZeroMemory(&shaderParam, sizeof(shaderParam));

    int numComponents = 0;

    if (parameter.Mask & 0x01)
        numComponents++;
    if (parameter.Mask & 0x02)
        numComponents++;
    if (parameter.Mask & 0x04)
        numComponents++;
    if (parameter.Mask & 0x08)
        numComponents++;

    switch (parameter.ComponentType)
    {
    case D3D_REGISTER_COMPONENT_FLOAT32:
        switch (numComponents)
        {
        case 1: shaderParam.Type = ShaderParameter::Type::Float; break;
        case 2: shaderParam.Type = ShaderParameter::Type::Float2; break;
        case 3: shaderParam.Type = ShaderParameter::Type::Float3; break;
        case 4: shaderParam.Type = ShaderParameter::Type::Float4; break;
        default:
            Assert("Unsupported number of parameters!" && false);
            break;
        }
        break;

    default:
        Assert("Unsupported component type!" && false);
        break;
    }

    if (_stricmp(parameter.SemanticName, "POSITION") == 0)
    {
        shaderParam.Semantic = ShaderParameter::SemanticEnum::Position;
    }
    else if (_stricmp(parameter.SemanticName, "NORMAL") == 0)
    {
        shaderParam.Semantic = ShaderParameter::SemanticEnum::Normal;
    }
    else if (_stricmp(parameter.SemanticName, "TANGENT") == 0)
    {
        shaderParam.Semantic = ShaderParameter::SemanticEnum::Tangent;
    }
    else if (_stricmp(parameter.SemanticName, "BITANGENT") == 0)
    {
        shaderParam.Semantic = ShaderParameter::SemanticEnum::BiTangent;
    }
    else if (_stricmp(parameter.SemanticName, "TEXCOORD") == 0)
    {
        shaderParam.Semantic = ShaderParameter::SemanticEnum::TexCoord;
    }
    else
    {
        Assert("Unsupported semantic name!" && false);
    }

    shaderParam.Index = static_cast<ushort>(parameter.SemanticIndex);

    return shaderParam;
}


