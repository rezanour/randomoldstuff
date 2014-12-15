#include "q2shadercommon.hlsli"

struct DirectionalLight
{
    float4 Direction;       // Must be normalized!
    float4 Color;
};

cbuffer PSGlobals
{
    DirectionalLight Light1;
    DirectionalLight Light2;
    DirectionalLight Light3;
};

sampler Sampler;
Texture2D Texture;

float4 ComputeDirectionalLight(
    in float3 normal,           // the world space normal of the surface being lit (already normalized)
    in DirectionalLight light   // a directional light
    )
{
    float NdL = dot(normal, light.Direction.xyz);
    return light.Color * saturate(NdL);
}

float4 main(VS_OUTPUT input) : SV_TARGET
{
    // Sample the texture to get our base surface color
    float4 sample = Texture.Sample(Sampler, input.UV);

    // Normalize since normal gets interpolated and can be outside of the [0, 1] range
    //float3 normal = normalize(input.Normal);

    // Apply the lights and return the final color
    return sample;// * (ComputeDirectionalLight(normal, Light1) + ComputeDirectionalLight(normal, Light2) + ComputeDirectionalLight(normal, Light3));
}
