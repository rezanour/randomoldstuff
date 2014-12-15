// NOTE: These L3DGraphics-specific serializers need a better home outside of ContentStudio
#pragma once

#include <stde\types.h>
#include <stde\com_ptr.h>
#include <vector>

#include <GDK\Shaders.h>
#include <d3d11.h>

// The namespace is L3DGraphics, because this is not actually part of CoreServices. It's specific to our graphics plugin only
namespace L3DGraphics
{
    class GeometryData
    {
    public:
        static HRESULT GDK_API Save(_In_ stde::com_ptr<IStream> spStream, 
                                    _In_ const std::vector<byte>& vertices, _In_ size_t numVertices, _In_ size_t vertexStride, _In_ const std::vector<GDK::ShaderParameter>& vertexLayout, 
                                    _In_ const std::vector<uint32>& indices, _In_ size_t indexStride, _In_ D3D11_PRIMITIVE_TOPOLOGY topology);

        static HRESULT GDK_API Load(_In_ stde::com_ptr<IStream> spStream, 
                                    _Out_ std::vector<byte>& vertices, _Out_ size_t& numVertices, _Out_ size_t& vertexStride, _Out_ std::vector<GDK::ShaderParameter>& vertexLayout, 
                                    _Out_ std::vector<uint32>& indices, _Out_ size_t& indexStride, _Out_ D3D11_PRIMITIVE_TOPOLOGY& topology);

        static HRESULT Load2(_In_ stde::com_ptr<IStream> spStream, _Out_ size_t* pStride, _Out_ size_t* pVertexCount, _Out_ std::vector<byte>& vertexData, _Out_ std::vector<uint32>& indices);
    };

    class TextureData
    {
    public:
        static HRESULT GDK_API Load(_In_ stde::com_ptr<IStream>& spStream,
                                    _In_ stde::com_ptr<ID3D11Device>& spDevice,
                                    _Deref_out_ ID3D11ShaderResourceView** ppView);
    };

    class VertexShaderParser
    {
    public:
        static HRESULT GDK_API Parse(_In_ const std::string& shaderCode, _In_ const std::string& entryPoint, _In_ const std::string& target,
                                     _Out_ std::vector<GDK::ShaderParameter>& signature, 
                                     _Out_ std::vector<GDK::ShaderConstant>& constants);
    };
}
