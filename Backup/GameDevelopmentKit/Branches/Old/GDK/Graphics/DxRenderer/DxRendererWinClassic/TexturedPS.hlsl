#include "ShaderCommon.hlsli"

//
// Pixel shaders
//

float4 TexturedPS(VSOutput input) : SV_TARGET0
{
    // Sample the texture to get our base surface color
    float4 sample = Texture.Sample(Sampler, input.UV);

    // Normalize since normal gets interpolated and can be outside of the [0, 1] range
    float3 normal = normalize(input.WorldNormal);

    // Apply the lights and return the final color
    return sample * (ComputeDirectionalLight(normal, Light1) + ComputeDirectionalLight(normal, Light2));
}
