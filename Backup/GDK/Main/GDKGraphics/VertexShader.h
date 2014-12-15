#pragma once

#include "GraphicsTypes.h"
#include <stde\non_copyable.h>
#include <vector>
#include <map>
#include "Renderer.h"

namespace Lucid3D
{
    enum ParameterType
    {
        Float,
        Float2,
        Float3,
        Float4,
    };

    enum ParameterSemantic
    {
        Position,
        Normal,
        Tangent,
        BiTangent,
        TextureCoord,
    };

    struct ShaderParameter
    {
        ParameterType Type;
        ParameterSemantic Semantic;
        unsigned int Index;
    };

    class VertexShader;
    typedef stde::ref_counted_ptr<VertexShader> VertexShaderPtr;


    class VertexShader : stde::non_copyable, public GDK::RefCountedBase<VertexShader>
    {
    public:
        HRESULT CreateCompatibleInputLayout(_In_ const std::vector<ShaderParameter>& vertexLayout, _Out_ InputLayoutPtr& spLayout);

        // Binds the shader to the context
        HRESULT Bind(_In_ Renderer* pRenderer);

    private:
        D3DVertexShaderPtr _spShader;
        std::vector<ShaderParameter> _signature;
    };
}

