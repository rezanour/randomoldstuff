//==============================================================================
//  PixelShader for rendering point lights.
//==============================================================================

#include "common.hlsli"

cbuffer psPerFrame : register(b0)
{
    float Projection33; // m33 of projection matrix
    float Projection43; // m43 of projection matrix
};

Texture2D DiffuseTexture : register (t0);
Texture2D NormalTexture : register (t1);
Texture2D DepthTexture : register (t2);

struct psPointLightInput
{
    float4 Position : SV_POSITION;

    float3 ViewPosition : POSITION0;
    float3 ViewLightPosition : POSITION1;
    float3 ViewRay : NORMAL;
    float3 Color : COLOR;
    float Radius : TEXCOORD;
};

float4 main(psPointLightInput input) : SV_TARGET
{
    int3 pixelCoord = GetPixelCoord(input.Position);

    float3 materialDiffuse = DiffuseTexture.Load(pixelCoord).xyz;
    float4 normalAndSpecular = NormalTexture.Load(pixelCoord);
    float logDepth = DepthTexture.Load(pixelCoord).x;

    float3 normal = normalize(UnpackVector(normalAndSpecular.xyz));
    float shininess = normalAndSpecular.w * MAX_SPECULAR;

    float3 viewPosition = ReconstructViewPosition(input.ViewRay, logDepth, Projection33, Projection43);

    float3 direction = input.ViewLightPosition - viewPosition;
    float dist = length(direction);
    direction = normalize(direction);

    float attenuation = saturate(1.0f - dist / input.Radius);

    float3 diffuse = ComputeDiffuse(direction, input.Color, normal);
    float3 specular = float3(0, 0, 0);

    if (shininess > 0)
    {
        specular = ComputeSpecular(direction, input.Color, normal, normalize(-input.ViewRay.xyz), shininess);
    }

    return float4(materialDiffuse * (diffuse + specular) * attenuation, 0);
}
