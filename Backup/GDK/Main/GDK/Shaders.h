// Shader related types
#pragma once

#include "Platform.h"

namespace GDK
{
    struct ShaderParameter
    {
        enum Type
        {
            Float,
            Float2,
            Float3,
            Float4,
            Float3x3,
            Float4x4,
        };

        enum SemanticEnum
        {
            Position,
            Normal,
            Tangent,
            BiTangent,
            TexCoord,
        };

        Type Type;
        SemanticEnum Semantic;
        unsigned short Index;
    };

    struct ShaderConstant
    {
        enum Type
        {
            Float,
            Float2,
            Float3,
            Float4,
            Float3x3,
            Float4x4,
            Bool,
            Int,
            UInt,
            Sampler,
            Texture2D,
            TextureCube,
        };

        enum Semantic
        {
            WorldMatrix,
            InvWorldMatrix,
            ViewMatrix,
            InvViewMatrix,
            ProjMatrix,
            InvProjMatrix,
            WorldViewProjMatrix,
            InvWorldViewProjMatrix,
            ViewProjMatrix,
            InvViewProjMatrix,
            LinearSampler,
            PointSampler,
            DiffuseTexture,
            NormalTexture,
            SpecularTexture,
        };

        Type Type;
        Semantic Semantic;
    };
}
