//==============================================================================
//  PixelShader for rendering geometry into the GBuffer
//==============================================================================

#include "common.hlsli"

struct psGBufferOutput
{
    float4 Diffuse : SV_TARGET0;
    float4 ViewNormal : SV_TARGET1;
};

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D SpecularMap : register(t2);

sampler DiffuseSampler : register(s0);
sampler NormalSampler : register(s1);
sampler SpecularSampler : register(s2);

psGBufferOutput main(psPositionNormalTangentInput input)
{
    psGBufferOutput output;

    output.Diffuse = float4(DiffuseMap.Sample(DiffuseSampler, input.TexCoord).xyz, 1);
    float3 normal = normalize(UnpackVector(NormalMap.Sample(NormalSampler, input.TexCoord).xyz));
    float specular = SpecularMap.Sample(SpecularSampler, input.TexCoord).x;

    float3x3 tangentFrame = float3x3(normalize(input.ViewTangent), normalize(input.ViewBitangent), normalize(input.ViewNormal));
    output.ViewNormal = float4(PackVector(normalize(mul(normal, tangentFrame)).xyz), specular);

    return output;
}
