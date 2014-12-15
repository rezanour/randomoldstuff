//==============================================================================
//  PixelShader for rendering directional lights
//==============================================================================

#include "common.hlsli"

#define LIGHTS_PER_DRAW 8

cbuffer psPerDraw
{
    float4 Direction[LIGHTS_PER_DRAW];
    float4 Color[LIGHTS_PER_DRAW];
    uint NumLights;
};

Texture2D DiffuseTexture;
Texture2D NormalTexture;

struct psDirectionalLightInput
{
    float4 Position : SV_POSITION;
    float3 ViewRay : POSITION;
};

float4 main(psDirectionalLightInput input) : SV_TARGET
{
    int3 pixelCoord = GetPixelCoord(input.Position);

    float3 materialDiffuse = DiffuseTexture.Load(pixelCoord).xyz;
    float4 normalAndSpecular = NormalTexture.Load(pixelCoord);

    float3 normal = normalize(UnpackVector(normalAndSpecular.xyz));
    float shininess = normalAndSpecular.w * MAX_SPECULAR;
    float3 view = normalize(-input.ViewRay.xyz);

    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);

    for (uint i = 0; i < NumLights; ++i)
    {
        diffuse += ComputeDiffuse(Direction[i].xyz, Color[i].xyz, normal);

        if (shininess > 0)
        {
            specular += ComputeSpecular(Direction[i].xyz, Color[i].xyz, normal, view, shininess);
        }
    }

    return float4(materialDiffuse * (diffuse + specular), 0);
}
